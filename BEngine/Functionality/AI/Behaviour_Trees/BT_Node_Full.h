//#pragma once
//#include <functional>	//Function pointers
//#include <vector>
//
///*
//	Complete, composite behavior node (will be broken down into individual nodes later)
//*/
//class BT_Node_Full
//{
//public:
//
//	//////////////////////////////
//	/// Enums, Types & Statics ///
//	//////////////////////////////
//
//	//Skip code used by condition code for use when resolution has no linked task/or any other time a node is invalid
//	static const unsigned INVALID_INDEX = 0xFFFFFFFF;
//
//	//Describes what type of node this is (should be automatically set by child class)
//	enum class NodeTypeID : unsigned short
//	{
//		ROOT,
//		ACTION,
//		CONDITION,
//		SELECTOR,
//		COMPOSITE,
//		SEQUENCE,
//		UNDEFINED
//	};
//
//	//Describes the current condition of the node or action under taken by the node
//	//Functions performing actions will need to return this type for tree functionality
//	enum class ConditionStatus : unsigned short
//	{
//		COMPLETE,
//		COMPLETE_UNMET_CONDITION,
//		INCOMPLETE,
//		CONDITION_RESOLVED,
//		ROOT_TREE_COMPLETE,
//		COUNT
//	};
//
//	//Default action function typedef. Binds the node to an actor via the target function to perform whatever behaviour required for that node.
//	typedef std::function<ConditionCodes()> NodeFunc;
//
//	////////////////////
//	/// Constructors ///
//	////////////////////
//
//	BT_Node_Full() {}
//	/*virtual*/ ~BT_Node_Full() {}
//
//	//////////////////
//	/// Operations ///
//	//////////////////
//
//	//
//	//Init
//	//
//
//	//
//	//Main Behaviour
//	//
//
//	AI_ConditionCodes RunNode();
//
//
//	//
//	//Main Behaviour
//	//
//
//
//
//	/////////////////
//	/// Accessors ///
//	/////////////////
//
//
//
//private:
//
//	//////////////////
//	/// Operations ///
//	//////////////////
//
//	AI_ConditionCodes Run_RootNode();
//	AI_ConditionCodes Run_ActionNode();
//	AI_ConditionCodes Run_CompositeNode();
//	AI_ConditionCodes Run_ConditionNode();
//	AI_ConditionCodes Run_SelectorNode();
//	AI_ConditionCodes Run_SequenceNode();
//
//	////////////
//	/// Data ///
//	////////////
//
//	//
//	//General Data
//	//
//
//	//What type of node this represents (should be automatically defined on creation)
//	NodeTypeID m_NodeID = NodeTypeID::UNDEFINED;
//
//	//Personal index in node array
//	unsigned m_ArrayIndex = INVALID_INDEX;
//	//Index to parent (should always be valid EXCEPT in the case of a root node)
//	unsigned m_ParentsArrayIndex = INVALID_INDEX;
//
//	//
//	//Shared Parent Data
//	//
//
//	//For parent nodes (Root/Composite/Condition/Sequence), Need to track more than one potential child node, so track their indexes via array
//	std::vector<unsigned> m_ChildIndexes;
//	//The currently active node being looked at/worked on by this parent node (relative the container above)
//	unsigned m_CurrentChildIndexPos = 0;
//
//	//
//	//Condition Code Specifics
//	//
//
//	//If flagged, then the condition will only check against one condition type (need to look into this)
//	bool m_SingleConOnly = false;
//	//If flagged, condition must return a code
//	bool m_SingleConRetReq = false;
//	//Flags that the node has specifically resolved a node to point to after condition resolution
//	ConditionStatus m_ConditionStatus = ConditionStatus::INCOMPLETE;
//
//};
