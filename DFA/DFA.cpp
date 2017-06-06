// DFA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "DFA.h"



int main()
{

    enum {
        stop,
        forward,
        up,
    };

    DFA::graph state_machine;

    DFA::state sforward{forward, "forward"};
    DFA::state sstop{stop, "stop"};
    DFA::state sup{up, "up"};

    DFA::node nforward(sforward);
    DFA::node nstop(sstop);
    DFA::node nup(sup);

    

    DFA::rule stop2forward;
    DFA::rule forward2up;
    DFA::rule up2forward;


    DFA::condition forward_available{forward,"forward", true};
    DFA::condition forward_invalid{forward,"forward", false};
    
    stop2forward.add(forward_available);
    forward2up.add(forward_invalid);
    up2forward.add(forward_available);

    nforward.add_edge(sup, forward2up, 0);
    nstop.add_edge(sforward, stop2forward, 0);
    nup.add_edge(sforward, up2forward, 0);


    state_machine.force_add(nforward);
    state_machine.force_add(nstop);
    state_machine.force_add(nup);

    DFA::context ctx1;
    ctx1.m_conds.push_back(forward_available);
    DFA::context ctx2;
    ctx2.m_conds.push_back(forward_invalid);


    auto next = state_machine[stop]->jump<DFA::similarity_match>(ctx1);
    next = state_machine[stop]->jump<DFA::similarity_match>(ctx2);
    next = state_machine[forward]->jump<DFA::similarity_match>(ctx1);
    next = state_machine[forward]->jump<DFA::similarity_match>(ctx2);
    next = state_machine[up]->jump<DFA::similarity_match>(ctx1);
    next = state_machine[up]->jump<DFA::similarity_match>(ctx2);


    return 0;
}

