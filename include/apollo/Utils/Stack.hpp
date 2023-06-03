/*
 * This file is part of the [Apollo-LS] project.
 * Copyright [2023] [Rui Zhang] and other contributors.
 * See the LICENSE file in the project root directory for the full license information.
 * 
 * The sharing rights for this project belong to all those who have contributed to the project.
 * 
 * Thanks to the following contributors for their dedication and support of this project:
 *   - Rui Zhang 
*/


# include <iostream>
# include <vector>

using namespace std;

class Stack {
    public:
        Stack() {}
        ~Stack() {}

        union stack_elements {
            int count_ptr;
            string * msg;
        };

        using stack_e = union stack_elements;
        vector<stack_e> stack_basic;

        void	init_stack	();
        void	push		(int);
        void	push		(string*);
        stack_e	pop			();
        stack_e	top			();
        bool	empty		();
        void	clear		();
};


void Stack::push (int ival) {
	stack_e e;
	e.count_ptr = ival;
	stack_basic.push_back(e);
}

void Stack::push (string * msg) {
	stack_e e;
	e.msg = msg;
	stack_basic.push_back(e);
}

Stack::stack_e Stack::pop () {
	if (stack_basic.size() == 0) {
		stack_e e;
		return e;
	}
	stack_e e = top();
	stack_basic.pop_back();
	return e;
}

Stack::stack_e Stack::top () {
	if (stack_basic.size() == 0) {
		stack_e e;
		return e;
	}
	return stack_basic.at(stack_basic.size() - 1);
}

bool Stack::empty () {
	return stack_basic.empty();
}

void Stack::clear () {
	stack_basic.clear();
}