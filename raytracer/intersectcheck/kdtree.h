#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include <memory>
#include "raytracer/utils/boundutils.h"
#include "raytracer/interface/intersect.h"

struct KDAccelNode;
class BaseRTShape;

// Credits to https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Kd-Tree_Accelerator

class KDAccelTree : public Intersect
{
public:
    KDAccelTree(std::vector<std::shared_ptr<BaseRTShape>> primitives);

    ~KDAccelTree();

    // Get the boundary of the total tree.
    Bound getWorldBound() const { return m_bound; }

    // Check if a ray intersects with the primitives in the tree.
    // @param ray The Ray object describing the ray in the WORLD SPACE.
    // @param oSurInteraction Upon return, contains the intersection information if there is one.
    // @return A boolean value indicating if there is an intersection with the primitives in the tree.
    virtual bool intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const override;

private:
    // Build a tree with the given value. The method is called in the constructor.
    // @param nodeNum The index of current node (in m_nodes).
    // @param nodeBound The boundary of current node in WORLD SPACE.
    // @param primitiveBound The boundaries of all the primitives in WORLD SPACE.
    // @param edges A buffer to store the BoundEdge on a particular axis of a boundary. The buffer must be have the minimum size of [3][2 * nPrimitive]
    // @param primNums A buffer containing the indices of primitives (in the m_primitives) that are inside the current node.
    // @param nPrimitive The number of primitives inside current node.
    // @param depth The maximum depth that can go further.
    // @param badSplitCount A counter that indicates how many bad splits have happened.
    // (The split resulting in a greater cost than the original one of the evalution function is called a bad split.)
    // @param primIdxSec0Buffer A buffer for the function to work with to store primitives in below child. The buffer must be have the minimum size of [nPrimitive].
    // @param primIdxSec1Buffer A buffer for the function to work with to store primitives in above child. The buffer must be have the minimum size of [(maxDepth + 1) * nPrimitive].
    void buildTree(int nodeNum, Bound nodeBound,
                   const std::vector<Bound> &primitiveBounds,
                   std::vector<std::vector<BoundEdge>> &edges,
                   int *primNums, int nPrimitive, int depth, int badSplitCount,
                   int *primIdxSec0Buffer, int *primIdxSec1Buffer);

private:
    // The boundary of the total tree in WORLD SPACE.
    Bound m_bound;

    // The list of primitives of the tree.
    std::vector<std::shared_ptr<BaseRTShape>> m_primitives;

    // A list of primitive indices. This member is quite confusing and must be considered together with KDAccelNode to make any sense.
    // It's a buffer that contains the indices of primitives for the leaf nodes that have multiple primitives.
    // The leaf node with multiple primitive has two variables, nPrimitives and offset.
    // Hence the interval [offset, offset + nPrimitives) in m_primitiveIndices gives the indices of primitives that belong to this leaf node.
    std::vector<int> m_primitiveIndices;

    // All the tree nodes of the KD Tree.
    // Instead of building a common binary tree with two children pointers, the KD Tree nodes are store in a contiguous array.
    // The node for the area below a splitting plane is called belowChildNode, and similar naming principle for aboveChildNode
    // The belowChildNode is placed exactly 1 index behind its parent, while the aboveChildNode needs an offset (from the begin of the list) to indicates its location.
    // The offset is generated during the tree building process.
    std::vector<KDAccelNode> m_nodes;

    // The number of nodes already allocated. (Not all of them may be used.)
    int m_allocatedNodes;
    // The index of the next free node. This value increases during the tree building process and gives the offset for the aboveChildNode.
    int m_nextFreeNode;
    // We don't want the tree to go too deep, so there is a limitation on the maximum value of depth.
    int m_maxDepth;
};

#endif // KDTREE_H
