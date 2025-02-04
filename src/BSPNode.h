// BSP node class for BSP trees
// Written by Dr. Sergey G. Kosov in 2019 for Jacobs University
#pragma once

#include "types.h"
#include "BSPNode.h"
#include "BoundingBox.h"
#include "IPrim.h"
#include "ray.h"

class CBSPNode;
using ptr_bspnode_t = std::shared_ptr<CBSPNode>;

// ================================ BSP Node Class ================================
/**
 * @brief Binary Space Partitioning (BSP) node class
 */
class CBSPNode
{
public:
	/**
	 * @brief Leaf node constructor
	 * @param vpPrims The vector of pointers to the primitives included in the leaf node
	 */
	CBSPNode(const std::vector<ptr_prim_t>& vpPrims)
		: CBSPNode(vpPrims, 0, 0, nullptr, nullptr)
	{}
	/**
	 * @brief Branch node constructor
	 * @param splitDim The splitting dimension
	 * @param splitVal The splitting value
	 * @param left Pointer to the left sub-tree
	 * @param right Pointer to the right sub-tree
	 */
	CBSPNode(int splitDim, float splitVal, ptr_bspnode_t left, ptr_bspnode_t right)
		: CBSPNode(std::nullopt, splitDim, splitVal, left, right)
	{}
	CBSPNode(const CBSPNode&) = delete;
	virtual ~CBSPNode(void) = default;
	const CBSPNode& operator=(const CBSPNode&) = delete;

	/**
	 * @brief Traverses the ray \b ray and checks for intersection with a primitive
	 * @details If the intersection is found, \b ray.t is updated
	 * @param[in,out] ray The ray
	 * @param[in] t0 The distance from ray origin at which the ray enters the scene
	 * @param[in] t1 The distance from ray origin at which the ray leaves the scene
	 * @retval true If ray \b ray intersects any object
	 * @retval false otherwise
	 */
	bool intersect(Ray& ray, double t0, double t1) const
	{
		if (isLeaf()) {
			// --- PUT YOUR CODE HERE ---
			for (auto EachPrim : m_vpPrims) {
				//if (EachPrim->intersect(ray)) {
				EachPrim->intersect(ray);
					//return true;
				//}
			}
			return (ray.hit && ray.t < t1 + Epsilon);
			//return false;
		} 
		else {
		/*	
			Each branch node has two children, there are only 3 cases for sub - node intersections :
				intersecting with the left child only
				intersecting with the right child only
				both
			update ray.t
		*/
			// check for left and right child if anyone hits
			/*if (Left()->intersect(ray, t0, t1)) { return true; };
			if (Right()->intersect(ray, t0, t1)) { return true; };
			return false;*/

			//DistanceFromOriToSplitePlane and the value ray needs
			double Distance = m_splitVal - ray.org[m_splitDim];
			double RayValue = Distance / ray.dir[m_splitDim];
			//if the ray direction of the splitdimension is negative meaning the distance is also opposite
			/*if (ray.dir[m_splitDim] < 0) {
				auto FrontNode = Right() ;
				auto BackNode = Left();
			}
			else {
				auto FrontNode = Left();
				auto BackNode = Right();
			}*/
			auto FrontNode = (ray.dir[m_splitDim] < 0) ? Right() : Left();
			auto BackNode = (ray.dir[m_splitDim] < 0) ? Left() : Right();

			//consider three cases
			if (t0 >= RayValue) {
				//meaning the distance ray hit to the splite value is at the left/back
				return BackNode->intersect(ray, t0, t1);
			}
			else if (t1 <= RayValue) {
				return FrontNode->intersect(ray, t0, t1);
			}
			else {
				// travese both children. front one first, back one last
				if (FrontNode->intersect(ray, t0, RayValue))
					return true;
				return BackNode->intersect(ray, RayValue, t1);
			}
		}
		//if (isLeaf()) {
		//	for (auto& pPrim : m_vpPrims)
		//		pPrim->intersect(ray);
		//	return (ray.hit && ray.t < t1 + Epsilon);
		//}
		//else {
		//	// distance from ray origin to the split plane of the current volume (may be negative)
		//	double d = (m_splitVal - ray.org[m_splitDim]) / ray.dir[m_splitDim];

		//	auto frontNode = (ray.dir[m_splitDim] < 0) ? Right() : Left();
		//	auto backNode = (ray.dir[m_splitDim] < 0) ? Left() : Right();

		//	if (d <= t0) {
		//		// t0..t1 is totally behind d, only go to back side
		//		return backNode->intersect(ray, t0, t1);
		//	}
		//	else if (d >= t1) {
		//		// t0..t1 is totally in front of d, only go to front side
		//		return frontNode->intersect(ray, t0, t1);
		//	}
		//	else {
		//		// travese both children. front one first, back one last
		//		if (frontNode->intersect(ray, t0, d))
		//			return true;

		//		return backNode->intersect(ray, d, t1);
		//	}
		//}
	}

	/**
	 * @brief Returns the pointer to the \a left child
	 * @returns The pointer to the root-node of the \a left sub-tree
	 */
	ptr_bspnode_t Left(void) const { return m_pLeft; }
	/**
	 * @brief Returns the pointer to the \a right child
	 * @returns The pointer to the root-node of the \a right sub-tree
	 */
	ptr_bspnode_t Right(void) const { return m_pRight; }

	
private:
	/**
	 * @brief Genereal private constructor
	 * @details This constructor may contruct both types of the nodes (leaf and branch) and is to be called from public leaf- and branch-node contructors
	 * @param vpPrims The vector of pointers to the primitives included in the leaf node
	 * @param splitDim The splitting dimension
	 * @param splitVal The splitting value
	 * @param left Pointer to the left sub-tree
	 * @param right Pointer to the right sub-tree
	 */
	CBSPNode(std::optional<std::vector<ptr_prim_t>> vpPrims, int splitDim, float splitVal, ptr_bspnode_t left, ptr_bspnode_t right)
		: m_vpPrims(vpPrims.value_or(std::vector<ptr_prim_t>()))
		, m_splitDim(splitDim)
		, m_splitVal(splitVal)
		, m_pLeft(left)
		, m_pRight(right)
	{}

	/**
	 * @brief Checks whether the node is either leaf or branch node
	 * @retval true if the node is the leaf-node
	 * @retval false if the node is a branch-node
	 */
	bool isLeaf(void) const { return (!m_pLeft && !m_pRight); }
	
	
private:
	std::vector<ptr_prim_t>	m_vpPrims;		///< The vector of pointers to the primitives included in the leaf node
	int 					m_splitDim;		///< The splitting dimension
	float 					m_splitVal;		///< The splitting value
	ptr_bspnode_t 	        m_pLeft;		///< Pointer to the left sub-tree
	ptr_bspnode_t 	        m_pRight;		///< Pointer to the right sub-tree
};

