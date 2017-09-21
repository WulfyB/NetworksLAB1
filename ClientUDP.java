/*
*
* A Java remake of beej's talker.c, the "client" demo with additions for Lab 1.
* Group 13: Wulfy Boothe, Lane Little, David Harris
*/

import java.net.*; // for DatagramSocket, DatagramPacket, and InetAddress
import java.io.*; // for IOException
import java.text.DecimalFormat; //for Formating
public class ClientUDP { 

   private static final int TIMEOUT = 3000; // Resend timeout (milliseconds) 
   private static final int MAXTRIES = 5; // Maximum retransmissions 
   
   public static void main(String[] args) throws IOException {
   
      if (args.length > 4) 
      {
      // Test for correct # of args     
         System.err.println("Parameter(s): <Server> <Port> <Operation> <String>");
         return; 
      }
      boolean invalidOperation = !(args[2].equals("10") || args[2].equals("5") || args[2].equals("80"));
      if (invalidOperation)
      {
      //Tests for valid operations
         System.err.println("Please input a valid operation: 10, 5, or 80.");
         return;
      }
      int RID = 0x0D; //needs to increment somehow? I still don't know how to do this.
      
      String message = "";
      int count = 0;
      /*for (String temp : args) 
      {
         if (count < 3) 
         {
            count++;
            continue;
         }
         message += temp + " ";
      }
       */
      message = args[3];
      message = message.substring(0, message.length());
      System.out.println("Message: " + message);
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
      bytesToSend[2] = (byte)Integer.parseInt(args[2]);
      
      for(int i = 3; i < TML; i++)
      {
         bytesToSend[i] = bytesMessage[i - 3];//creates byte package
      }
      long startTime = System.nanoTime();
      try{ //try block for attempting to send and recieve the message
         InetAddress serverAddress = InetAddress.getByName(args[0]); // Server address 
      
      
      // Convert input String to bytes using the default character encoding 
      
         int servPort = Integer.parseInt(args[1]); //servPort as an integer 
      
      
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
               socket.receive(receivePacket); //recieves response from server
               receivedResponse = true;
            } 
            catch (SocketTimeoutException ste) 
            {
               tries ++; //increments tries
            }
         } while ((!receivedResponse) && (tries < MAXTRIES)); 
         if (receivedResponse)
         {
            long difference = System.nanoTime() - startTime;
            DecimalFormat frmt = new DecimalFormat("#.000");
            if (RID != receivePacket.getData()[1])
            {
               System.err.println("The server's returned RID does not match input RID. \nCheck program. Output received as follows:  \n\n");
               
            }
            if(args[2] == "5") 
            {
               System.out.println("RequestID: " + RID + "\nReceived: " + receivePacket.getData()[2]);
            }
            else {
               String receivedMessage = new String(receivePacket.getData(), 2, receivePacket.getData()[0] - 2);
               System.out.println("RequestID: " + RID + "\nReceived: " + receivedMessage);
               System.out.println("Round trip time: " + frmt.format(difference * 0.000001) + " milliseconds");
            }
         } 
         else 
            System.out.println("No response -- giving up."); 
         socket.close(); 
      
      }
      catch (UnknownHostException uhe)
      {
         System.err.println("Failed to find host.");
         return;
      }
      catch (SocketException se)
      {
         System.err.println("Failed to create socket.");
         return;
      }
      
   }
      
}
