#include "kdtree.h"
#include "raytracer/shapes/baseshape.h"
#include <stack>
#include <iostream>
#include <algorithm>

using namespace std;

struct KDAccelIntersectCheckNode {
    int idx;
    float tMin;
    float tMax;

    KDAccelIntersectCheckNode(int idx, float tMin, float tMax) : idx(idx), tMin(tMin), tMax(tMax) {}
};

struct KDAccelNode {
    union {
        float split;                    // Interior
        int onePrimitive;               // Leaf
        int primitiveIndicesOffset;     // Leaf
    };

private:
    union {
        int flags;                      // Both
        int nPrims;                     // Leaf
        int aboveChild;                 // Interior
    };

public:
    // KdAccelNode Methods
    void setupLeafNode(int *primNums, int nPrimitive, std::vector<int> &primitiveIndices) {
        flags = 3;
        nPrims |= (nPrimitive << 2);
        // Store primitive ids for leaf node
        if (nPrimitive == 0) {
            onePrimitive = 0;
        } else if (nPrimitive == 1) {
            onePrimitive = primNums[0];
        } else {
            primitiveIndicesOffset = primitiveIndices.size();
            for (int i = 0; i < nPrimitive; ++i) primitiveIndices.push_back(primNums[i]);
        }
    }

    void setupInteriorNode(int axis, float splitPos, int aboveChildOffset) {
        split = splitPos;
        flags = axis;
        aboveChild |= (aboveChildOffset << 2);
    }

    float getSplitPos() const { return split; }
    int getPrimitiveNum() const { return nPrims >> 2; }
    int getSplitAxis() const { return flags & 3; }
    int getAboveChild() const { return aboveChild >> 2; }
    bool isLeaf() const { return (flags & 3) == 3; }
};

KDAccelTree::KDAccelTree(vector<shared_ptr<BaseRTShape>> primitives)
    : m_primitives(move(primitives))
{
    auto startTS = std::chrono::system_clock::now();
    std::cout << std::endl << "Begin building KD Tree for " << m_primitives.size() << " primitives" << std::endl;

    // setup tree basic configuration
    m_nextFreeNode = m_allocatedNodes = 0;
    m_maxDepth = std::round(8 + 1.3f * log2(m_primitives.size()));

    // compute the bound of primitives for further calculation
    vector<Bound> primitiveBounds;
    for (const shared_ptr<BaseRTShape> &prim : m_primitives) {
        Bound b = prim->getWorldBound();
        m_bound.unite(b);
        primitiveBounds.push_back(b);
    }

    // allocate working memory for tree construction
    // The edge buffer for 3 axis
    vector<vector<BoundEdge>> edges(3, vector<BoundEdge>(2 * m_primitives.size()));

    std::unique_ptr<int[]> primIdxSec0Buffer(new int[m_primitives.size()]);
    std::unique_ptr<int[]> primIdxSec1Buffer(new int[(m_maxDepth + 1) * m_primitives.size()]);

    // The indices of each primitive
    std::unique_ptr<int[]> primNums(new int[m_primitives.size()]);
    for (size_t i = 0; i < m_primitives.size(); i++) {
        primNums[i] = i;
    }

    // build tree
    buildTree(0, m_bound,
              primitiveBounds,
              edges,
              primNums.get(), m_primitives.size(), m_maxDepth, 0,
              primIdxSec0Buffer.get(), primIdxSec1Buffer.get());

    auto endTS = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTS-startTS;
    std::cout << "Complete building KD Tree. Time elapse: " << elapsed_seconds.count() * 1000.f << "ms" << std::endl;
}

KDAccelTree::~KDAccelTree() {

}

void KDAccelTree::buildTree(int nodeNum, Bound nodeBound,
                            const std::vector<Bound> &primitiveBounds,
                            std::vector<std::vector<BoundEdge>> &edges,
                            int *primNums, int nPrimitive, int depth, int badSplitCount,
                            int *primIdxSec0Buffer, int *primIdxSec1Buffer) {
    // If all the nodes are used up, then expand the treeNode array
    if (m_nextFreeNode == m_allocatedNodes) {
        int nodeCount = std::max(2 * m_allocatedNodes, 512);
        m_nodes.resize(nodeCount);
        m_allocatedNodes = nodeCount;
    }
    m_nextFreeNode += 1;

    // If there is only one primitive left or the maximum depth has been reached, terminate the recursion.
    if (nPrimitive <= 1 || depth == 0) {
        m_nodes[nodeNum].setupLeafNode(primNums, nPrimitive, m_primitiveIndices);
        return;
    }

    // Otherwise, setup the interior node and continue recursion

    // Choose the split axis and its position for the interior node.
    // bestAxis is namely the best axis that we choose for the next split.
    // bestBoundEdgeIdx is the index of the BoundEdge that we choose for the next split.
    // oldCost means the cost before the node is split, which is exactly nPrimitive.
    int bestAxis = -1, bestBoundEdgeIdx = -1;
    float bestCost = FLT_MAX;
    float oldCost = nPrimitive;
    float totalSurfaceArea = m_bound.surfaceArea();
    float invTotalSA = 1.f / totalSurfaceArea;

    glm::vec4 d = nodeBound.pMax - nodeBound.pMin;

    // Iterate through x, y, and z axis, labeled by 0, 1 and 2.
    for (int axis = 0; axis < 3; axis++) {
        // for each axis, we evaluate the split position by first mapping the bounding box into the selected axis.
        // Then sort the array in ascending order.
        for (int i = 0; i < nPrimitive; i++) {
            int primIdx = primNums[i];
            edges[axis][2 * i] = BoundEdge(primitiveBounds[primIdx].pMin[axis], primIdx, true);
            edges[axis][2 * i + 1] = BoundEdge(primitiveBounds[primIdx].pMax[axis], primIdx, false);
        }

        // we only need to sort the interval of [0, 2 * nPrimitive)
        std::sort(edges[axis].begin(), edges[axis].begin() + 2 * nPrimitive, [](const BoundEdge &a, const BoundEdge &b) {
            return a.t < b.t || (a.t == b.t && (int)a.type < (int)b.type);
        });

        // nBelow stands for the number of primitives that are below the split plane.
        // nAbove stands for the number of primitives that are above the split plane.
        // The sum of these two numbers can exceed nPrimitive since a primitive can be on both sides of the split plane.
        int nBelow = 0, nAbove = nPrimitive;
        for (int i = 0; i < 2 * nPrimitive; i++) {
            if (edges[axis][i].type == EdgeType::END) {
                // if we are drawing a plane at the end point a primitive box on a specific axis,
                // then this primitive should be positioned below the split plane.
                // Hence, nAbove should minus 1.
                nAbove -= 1;
            }

            // The actual split position
            float edgeT = edges[axis][i].t;

            // we only calculate the cost if the split position choice lies inside the boundary of current node
            if (edgeT > nodeBound.pMin[axis] && edgeT < nodeBound.pMax[axis]) {
                // calculate the surface areas for evaluation
                int axis1 = (axis + 1) % 3, axis2 = (axis + 2) % 3;
                float belowSA = 2 * (d[axis1] * d[axis2] + (d[axis1] + d[axis2]) * (edgeT - nodeBound.pMin[axis]));
                float aboveSA = 2 * (d[axis1] * d[axis2] + (d[axis1] + d[axis2]) * (nodeBound.pMax[axis] - edgeT));

                // calculate the percentage
                belowSA *= invTotalSA;
                aboveSA *= invTotalSA;
                float cost = nBelow * belowSA + nAbove * aboveSA;
                if (cost < bestCost) {
                    bestCost = cost;
                    bestAxis = axis;
                    bestBoundEdgeIdx = i;
                }
            }

            // Further from this point, any split position we choose on the current axis
            // will be greater than the start point of current primitive,
            // which means the primitive will be place below the split plane, partially or totally.
            // Thus, we need to add one to nBelow.
            if (edges[axis][i].type == EdgeType::START) {
                nBelow += 1;
            }
        }
    }

    // If the best split we can find is still greater than the old cost, increase badSplitCount by 1.
    if (bestCost > oldCost) {
        badSplitCount += 1;
    }

    // Create a leaf node immediately under the following conditions
    // 1. No available split plane is found.
    // 2. There has been 3 bad split accumulated on current subtree.
    // 3. The bad split is signficant worse than the original one.
    if (bestAxis == -1 || badSplitCount == 3 || (bestCost > 4 * oldCost && nPrimitive < 16)) {
        m_nodes[nodeNum].setupLeafNode(primNums, nPrimitive, m_primitiveIndices);
        return;
    }

    // Classify the primitive using the split plane we just chose.
    // We skip i == bestBoundEdgeIdx so that the primitive we choose for drawing the split plane will only exist in one side.
    // Other primitives may appear only in one side or both sides depending its position with respect to the split plane.
    int nSec0 = 0, nSec1 = 0;
    for (int i = 0; i < bestBoundEdgeIdx; i++) {
        if (edges[bestAxis][i].type == EdgeType::START) {
            primIdxSec0Buffer[nSec0++] = edges[bestAxis][i].primNum;
        }
    }

    for (int i = bestBoundEdgeIdx + 1; i < 2 * nPrimitive; i++) {
        if (edges[bestAxis][i].type == EdgeType::END) {
            primIdxSec1Buffer[nSec1++] = edges[bestAxis][i].primNum;
        }
    }

    // Now, recursively construct children nodes.
    float splitPos = edges[bestAxis][bestBoundEdgeIdx].t;

    Bound boundSec0 = nodeBound, boundSec1 = nodeBound;
    boundSec0.pMax[bestAxis] = splitPos;
    boundSec1.pMin[bestAxis] = splitPos;

    // The node representing primitives below the split plane is place right next to its parent.
    buildTree(nodeNum + 1, boundSec0, primitiveBounds, edges,
              primIdxSec0Buffer, nSec0, depth - 1, badSplitCount,
              primIdxSec0Buffer, primIdxSec1Buffer + nPrimitive);
    // The node representing primitives above the split plane is recorded with an offset.
    int aboveChildOffset = m_nextFreeNode;
    m_nodes[nodeNum].setupInteriorNode(bestAxis, splitPos, aboveChildOffset);

    buildTree(aboveChildOffset, boundSec1, primitiveBounds, edges,
              primIdxSec1Buffer, nSec1, depth - 1, badSplitCount,
              primIdxSec0Buffer, primIdxSec1Buffer + nPrimitive);
}

bool KDAccelTree::intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const {
    // The 't' stands for the extent a ray travels starting from its eyePoint.
    float tMin = 0, tMax = 0;
    // If no intersection with current node, then return immediately.
    if (!m_bound.intersect(ray, tMin, tMax)) {
        return false;
    }

    // Calculate the inverted value of direction to save time on division.
    glm::vec4 invDirection = glm::vec4(1. / ray.direction.x, 1. / ray.direction.y, 1. / ray.direction.z, 0.f);

    bool hit = false;

    // use stack to traverse the tree.
    stack<KDAccelIntersectCheckNode> st;
    KDAccelIntersectCheckNode checkNode(0, tMin, tMax);
    st.push(checkNode);

    while (!st.empty()) {
        KDAccelIntersectCheckNode &checkNode = st.top();
        st.pop();

        const KDAccelNode &curNode = m_nodes[checkNode.idx];
        tMin = checkNode.tMin;
        tMax = checkNode.tMax;

        // If the ray can't even reach the bound of current node, then we should terminate the loop,
        // since we are pushing node into the stack in the order of ray encounter order.
        if (ray.tMax < tMin) break;

        // Visit the node
        if (curNode.isLeaf()) {
            // The node is a leaf node.
            int nPrimitives = curNode.getPrimitiveNum();
            if (nPrimitives == 1) {
                // The node has only one primitive, use the data structure specialized for this case.
                const std::shared_ptr<BaseRTShape> &shape = m_primitives[curNode.onePrimitive];

                if (shape->intersect(ray, oSurInteraction)) {
                    hit = true;
//                    intersection.primitive = shape;
                }
            } else {
                // Otherwise, obtain the list of primitives, and check intersection.
                for (int i = 0; i < nPrimitives; i++) {
                    int idx = m_primitiveIndices[curNode.primitiveIndicesOffset + i];
                    const std::shared_ptr<BaseRTShape> &shape = m_primitives[idx];

                    if (shape->intersect(ray, oSurInteraction)) {
                        hit = true;
//                        intersection.primitive = shape;
                    }
                }
            }
        } else {
            // Compute the t value that let the ray reaches the split plane
            int axis = curNode.getSplitAxis();
            float tSplit = (curNode.getSplitPos() - ray.origin[axis]) * invDirection[axis];

            // Determine which one ray intersects first, we determine this by evaluating the eyePoint position of the ray with respect to the split plane.
            int firstChild, secondChild;
            // 1. The eyePoint value on split axis is less than the split plane position
            // 2. The eyePoint of the ray is exactly on the split plane, but the ray is pointing is the negative direction of the split axis.
            int belowFirst = ray.origin[axis] < curNode.getSplitPos() || (ray.origin[axis] == curNode.getSplitPos() && ray.direction[axis] <= 0);

            if (belowFirst) {
                firstChild = checkNode.idx + 1;
                secondChild = curNode.getAboveChild();
            } else {
                firstChild = curNode.getAboveChild();
                secondChild = checkNode.idx + 1;
            }

            KDAccelIntersectCheckNode checkNodeFirst = KDAccelIntersectCheckNode(firstChild, tMin, tSplit);
            KDAccelIntersectCheckNode checkNodeSecond = KDAccelIntersectCheckNode(secondChild, tSplit, tMax);
            // There are also cases that we only need to expand to only one of the children.
            if (tSplit > tMax || tSplit <= 0) {
                // 1. The ray intersects the splitting plane beyonds its tMax intersection point with the node boundary. (tSplit > tMax)
                // 2. The ray is facing away from the splitting plane. (tSplit <= 0)

                // reset the tMax of the node to current tMax
                checkNodeFirst.tMax = tMax;
                st.push(checkNodeFirst);
            } else if (tSplit < tMin) {
                // 1. The ray hits the splitting before it intersects with the node boundary. (tSplit < tMin)

                // reset the tMin of the node to current tMin
                checkNodeSecond.tMin = tMin;
                st.push(checkNodeSecond);
            } else {
                // Push both children into the stack, we would like to visit 'firstChild' first so the push order is reversed.
                st.push(checkNodeSecond);
                st.push(checkNodeFirst);
            }
        }
    }
    return hit;
}
