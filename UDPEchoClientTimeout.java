/*
*
* A Java remake of beej's talker.c, the "client" demo with additions for Lab 1.
* Group 13: Wulfy Boothe, Lane Little, David Harris
*/

import java.net.*; // for DatagramSocket, DatagramPacket, and InetAddress
import java.io.*; // for IOException
public class UDPEchoClientTimeout { 

   private static final int TIMEOUT = 3000; // Resend timeout (milliseconds) 
   private static final int MAXTRIES = 5; // Maximum retransmissions 
   
   public static void main(String[] args) {
   
      if (args.length > 4) 
      {
      // Test for correct # of args     
         System.err.println("Parameter(s): <Client> <Server> <Port> <Operation> <String>");
         return; 
      }
      boolean invalidOperation = (args[3] != "10" && args[3] != "5" && args[3] != "80");
      if (invalidOperation)
      {
      //Tests for valid operations
         System.err.println("Please input a valid operation: 10, 5, or 80.");
         return;
      }
      int RID = 0; //needs to increment somehow? 
      
      String message = "";
      int count = 0;
      for (String temp : args) 
      {
         if (count != 3) 
         {
            count++;
            continue;
         }
         message += temp + " ";
      }
      message = message.substring(0, message.length() - 1);
      byte[] bytesMessage = message.getBytes(); 
      boolean sizeInvalid = (bytesMessage.length > 252);
      if (sizeInvalid)
      {
      //size test
         System.err.println("The message cannot exceed 252 characters");
         return;
      }
      int TML = 3 + bytesMessage.length;
      byte[] bytesToSend = new byte[TML];
      bytesToSend[0] = (byte)TML;
      bytesToSend[1] = (byte)RID;
      bytesToSend[2] = (byte)Integer.parseInt(args[3]);
      
      for(int i = 3; i < TML; i++)
      {
         bytesToSend[i] = bytesMessage[i - 3];
      }
      InetAddressserverAddress = InetAddress.getByName(args[1]); // Server address 
      // Convert input String to bytes using the default character encoding  
      
      intservPort = args[2]; 
      
      DatagramSocket socket = new DatagramSocket();
      socket.setSoTimeout(TIMEOUT); // Maximum receive blocking time (milliseconds) 
      DatagramPacket sendPacket = new DatagramPacket(bytesToSend, // Sending packet 
         bytesToSend.length, serverAddress, servPort);
      DatagramPacket receivePacket = // Receiving packet         
         new DatagramPacket(new byte[bytesToSend.length], bytesToSend.length);
      int tries = 0; // Packets may be lost, so we have to keep trying 
      boolean receivedResponse = false; 
      do 
      { 
         socket.send(sendPacket);
         try   
         {
            socket.receive(receivePacket);
            receivedResponse = true;
         } 
         catch (SocketTimeoutExdception ste) 
         {
            tries ++;
            //do nothing
         }
         //MANAGE TIMEOUTS
      } while ((!receivedResponse) && (tries < MAXTRIES)); 
      if (receivedResponse) 
         System.out.println("Received: " + new String(receivePacket.getData())); 
      else 
         System.out.println("No response -- giving up."); 
      socket.close(); }
}