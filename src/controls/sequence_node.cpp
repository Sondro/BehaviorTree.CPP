/* Copyright (C) 2015-2018 Michele Colledanchise -  All Rights Reserved
 * Copyright (C) 2018 Davide Faconti -  All Rights Reserved
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
*   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
*   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "behavior_tree_core/controls/sequence_node.h"

BT::SequenceNode::SequenceNode(const std::string& name) : ControlNode::ControlNode(name, NodeParameters())
{
}

BT::NodeStatus BT::SequenceNode::tick()
{
    // gets the number of children. The number could change if, at runtime, one edits the tree.
    const unsigned N_of_children = children_nodes_.size();

    // Routing the ticks according to the sequence node's logic:

    setStatus(NodeStatus::RUNNING);

    for (unsigned int i = 0; i < N_of_children; i++)
    {
        TreeNode* child_node = children_nodes_[i];

        const NodeStatus child_status = child_node->executeTick();

        // Ponderate on which status to send to the parent
        if (child_status != NodeStatus::SUCCESS)
        {
            // If the  child status is not success, halt the next children and return the status to your parent.
            if (child_status == NodeStatus::FAILURE)
            {
                for (unsigned t = 0; t <= i; t++)
                {
                    children_nodes_[t]->setStatus(NodeStatus::IDLE);
                }
            }

            haltChildren(i + 1);
            return child_status;
        }
        else
        {
            if (i == N_of_children - 1)
            {
                // If the  child status is success, and it is the last child to be ticked,
                // then the sequence has succeeded.
                for (auto& ch : children_nodes_)
                {
                    ch->setStatus(NodeStatus::IDLE);
                }
                return NodeStatus::SUCCESS;
            }
        }
    }
    throw std::runtime_error("This is not supposed to happen");
}