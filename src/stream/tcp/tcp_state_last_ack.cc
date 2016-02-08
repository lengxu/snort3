//--------------------------------------------------------------------------
// Copyright (C) 2015-2016 Cisco and/or its affiliates. All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------

// tcp_state_last_ack.cc author davis mcpherson <davmcphe@cisco.com>
// Created on: Aug 5, 2015

#include <iostream>
using namespace std;

#include "tcp_module.h"
#include "tcp_tracker.h"
#include "tcp_session.h"
#include "tcp_normalizer.h"
#include "tcp_state_last_ack.h"

#ifdef UNIT_TEST
#include "catch/catch.hpp"
#endif

TcpStateLastAck::TcpStateLastAck(TcpStateMachine& tsm, TcpSession& ssn) :
    TcpStateHandler(TcpStreamTracker::TCP_LAST_ACK, tsm), session(ssn)
{
}

TcpStateLastAck::~TcpStateLastAck()
{
}

bool TcpStateLastAck::syn_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    trk.s_mgr.sub_state |= SUB_SYN_SENT;

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::syn_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    trk.normalizer->ecn_tracker(tsd.get_tcph(), session.config->require_3whs() );
    if ( tsd.get_seg_len() )
        session.handle_data_on_syn(tsd);

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::syn_ack_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    trk.s_mgr.sub_state |= ( SUB_SYN_SENT | SUB_ACK_SENT );

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::syn_ack_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::ack_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    trk.update_tracker_ack_sent(tsd);

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::ack_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    trk.update_tracker_ack_recv(tsd);
    if ( SEQ_EQ(tsd.get_seg_ack(), trk.get_snd_nxt() ) )
        trk.set_tcp_state(TcpStreamTracker::TCP_CLOSED);

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::data_seg_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    trk.update_tracker_ack_sent(tsd);

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::data_seg_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    trk.update_tracker_ack_recv(tsd);
    if ( SEQ_EQ(tsd.get_seg_ack(), trk.get_snd_nxt() ) )
        trk.set_tcp_state(TcpStreamTracker::TCP_CLOSED);

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::fin_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    trk.update_tracker_ack_sent(tsd);

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::fin_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    trk.update_tracker_ack_recv(tsd);
    if ( SEQ_EQ(tsd.get_seg_ack(), trk.get_snd_nxt() ) )
        trk.set_tcp_state(TcpStreamTracker::TCP_CLOSED);

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::rst_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk);
}

bool TcpStateLastAck::rst_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    auto& trk = static_cast< TcpTracker& >( tracker );

    if ( trk.update_on_rst_recv(tsd) )
    {
        session.update_session_on_rst(tsd, false);
        session.update_perf_base_state(TcpStreamTracker::TCP_CLOSING);
        session.set_pkt_action_flag(ACTION_RST);
    }
    else
    {
        session.tel.set_tcp_event(EVENT_BAD_RST);
    }

    return default_state_action(tsd, trk);
}

