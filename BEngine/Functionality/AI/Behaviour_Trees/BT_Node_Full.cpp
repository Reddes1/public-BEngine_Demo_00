//#include "BT_Node_Full.h"
//
//#include "AI_Shared.h"
//
//
//AI_ConditionCodes BT_Node_Full::RunNode()
//{
//	//Operate differently based on node type
//	switch (m_NodeID)
//	{
//	case NodeTypeID::ACTION:
//		return AI_ConditionCodes();
//		break;
//
//	case NodeTypeID::ROOT:
//		return AI_ConditionCodes();
//		break;
//
//	case NodeTypeID::COMPOSITE:
//		return AI_ConditionCodes();
//		break;
//
//	case NodeTypeID::CONDITION:
//		return AI_ConditionCodes();
//		break;
//
//	case NodeTypeID::SELECTOR:
//		return AI_ConditionCodes();
//		break;
//	}
//
//	return AI_ConditionCodes();
//}
//
//AI_ConditionCodes BT_Node_Full::Run_RootNode()
//{
//	//Increment the internal array index
//	++m_CurrentChildIndexPos;
//
//	//Check to see if root has executed all child nodes
//	if (m_CurrentChildIndexPos >= m_ChildIndexes.size())
//	{
//		//Return that the root node is complete
//		return AI_ConditionCodes::BT_PARENT_COMPLETE;
//	}
//	//Root is still running, so signal accordingly
//	return AI_ConditionCodes::BT_PARENT_RUNNING;
//}
//
//AI_ConditionCodes BT_Node_Full::Run_ActionNode()
//{
//	//Run the function and defer the return code
//	return m_Func();
//}
//
//AI_ConditionCodes BT_Node_Full::Run_CompositeNode()
//{
//	//Increment the internal array index
//	++m_CurrentChildIndexPos;
//
//	//Check to see if root has executed all child nodes
//	if (m_CurrentChildIndexPos >= m_ChildIndexes.size())
//	{
//		//Return that the composite node is complete
//		return AI_ConditionCodes::BT_PARENT_COMPLETE;
//	}
//	//Parent is still running, so signal accordingly
//	return AI_ConditionCodes::BT_PARENT_RUNNING;
//}
//
//AI_ConditionCodes BT_Node_Full::Run_ConditionNode()
//{
//	return AI_ConditionCodes();
//}
//
//AI_ConditionCodes BT_Node_Full::Run_SelectorNode()
//{
//	return AI_ConditionCodes();
//}
//
//AI_ConditionCodes BT_Node_Full::Run_SequenceNode()
//{
//	return AI_ConditionCodes();
//}
