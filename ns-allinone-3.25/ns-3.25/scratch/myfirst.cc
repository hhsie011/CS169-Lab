/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <vector>

using namespace ns3;

int NUM_CLIENTS = 3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  uint32_t nPackets = 1;
  cmd.AddValue("nPackets", "Number of packets to echo", nPackets);
  cmd.Parse(argc, argv);  

  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NS_LOG_INFO("Creating Topology");
  
  NodeContainer nodes;
  nodes.Create (NUM_CLIENTS+1);

  PointToPointHelper pointToPoint;

  InternetStackHelper stack;
  stack.Install (nodes);
 
  NS_LOG_INFO("Assigning IP Addresses");

  Ipv4AddressHelper address1;
  address1.SetBase ("10.1.1.0", "255.255.255.0");

  NetDeviceContainer device1;
  device1 = pointToPoint.Install (nodes.Get(0), nodes.Get(1));
  Ipv4InterfaceContainer interface1 = address1.Assign(device1);

  Ipv4AddressHelper address2;
  address2.SetBase ("10.1.2.0", "255.255.255.0");
  
  NetDeviceContainer device2;
  device2 = pointToPoint.Install (nodes.Get(0), nodes.Get(2));    
  Ipv4InterfaceContainer interface2 = address2.Assign (device2);

  Ipv4AddressHelper address3;
  address3.SetBase("10.1.3.0", "255.255.255.0");

  NetDeviceContainer device3;
  device3 = pointToPoint.Install (nodes.Get(0), nodes.Get(3));
  Ipv4InterfaceContainer interface3 = address3.Assign (device3);

  std::vector<Ipv4InterfaceContainer> interfaces;
  interfaces.push_back(interface1); 
  interfaces.push_back(interface2);
  interfaces.push_back(interface3);  

  NS_LOG_INFO("Creating echoServer");

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  for (int i = 0; i < NUM_CLIENTS; ++i) {   
    UdpEchoClientHelper echoClient (interfaces[i].GetAddress(0), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (nPackets));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (16384)); 

    ApplicationContainer clientApps = echoClient.Install (nodes.Get (i+1));
    clientApps.Start (Seconds (2.0+i));
    clientApps.Stop (Seconds (30.0));
  }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
