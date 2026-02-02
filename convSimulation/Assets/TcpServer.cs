using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;

public class TcpServer
{
    private TcpListener tcpListener;
    private Thread listenerThread;
    private bool isRunning = false;
    private List<TcpClient> connectedClients = new List<TcpClient>();
    private Dictionary<string, bool> conveyorInbits = new Dictionary<string, bool>();
    private Dictionary<string, bool> conveyorOutbits = new Dictionary<string, bool>(); // Store outbit states
    private int port;

    public TcpServer(int serverPort)
    {
        port = serverPort;
    }

    public void Start()
    {
        if (isRunning)
        {
            Debug.LogWarning("Server is already running");
            return;
        }

        listenerThread = new Thread(new ThreadStart(ListenForClients));
        listenerThread.IsBackground = true;
        listenerThread.Start();
        isRunning = true;
        Debug.Log($"TCP Server started on port {port}");
    }

    public void Stop()
    {
        isRunning = false;

        foreach (var client in connectedClients)
        {
            if (client != null)
            {
                client.Close();
            }
        }
        connectedClients.Clear();

        if (tcpListener != null)
        {
            tcpListener.Stop();
        }

        Debug.Log("TCP Server stopped");
    }

    public void SetInbitStatus(string conveyorId, bool status)
    {
        conveyorInbits[conveyorId] = status;
    }

    public bool GetOutbitStatus(string conveyorId)
    {
        if (conveyorOutbits.ContainsKey(conveyorId))
        {
            return conveyorOutbits[conveyorId];
        }
        return false; // Default to false if not set
    }

    private void ListenForClients()
    {
        try
        {
            tcpListener = new TcpListener(IPAddress.Any, port);
            tcpListener.Start();

            while (isRunning)
            {
                TcpClient client = tcpListener.AcceptTcpClient();
                connectedClients.Add(client);
                Debug.Log("Client connected!");

                Thread clientThread = new Thread(new ParameterizedThreadStart(HandleClient));
                clientThread.IsBackground = true;
                clientThread.Start(client);
            }
        }
        catch (SocketException e)
        {
            Debug.LogError($"SocketException: {e.Message}");
        }
        catch (Exception e)
        {
            Debug.LogError($"Exception: {e.Message}");
        }
    }

    private void HandleClient(object obj)
    {
        TcpClient client = (TcpClient)obj;
        NetworkStream stream = client.GetStream();
        byte[] buffer = new byte[1024];

        try
        {
            while (isRunning && client.Connected)
            {
                int bytesRead = stream.Read(buffer, 0, buffer.Length);

                if (bytesRead > 0)
                {
                    string receivedData = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                    Debug.Log($"Received: {receivedData}");

                    string response = ProcessRequest(receivedData);

                    byte[] responseBytes = Encoding.UTF8.GetBytes(response);
                    stream.Write(responseBytes, 0, responseBytes.Length);
                    Debug.Log($"Sent: {response}");
                }
                else
                {
                    break;
                }
            }
        }
        catch (Exception e)
        {
            Debug.LogError($"Client handler error: {e.Message}");
        }
        finally
        {
            connectedClients.Remove(client);
            client.Close();
            Debug.Log("Client disconnected");
        }
    }

    private string ProcessRequest(string jsonRequest)
    {
        try
        {
            var request = JsonUtility.FromJson<OutbitRequest>(jsonRequest);

            if (request != null && request.type == "outbit")
            {
                Debug.Log($"Outbit request for conveyor '{request.conveyor}': {request.outbit}");

                // Store the outbit state
                conveyorOutbits[request.conveyor] = request.outbit;

                bool currentInbit = false;
                if (conveyorInbits.ContainsKey(request.conveyor))
                {
                    currentInbit = conveyorInbits[request.conveyor];
                }

                InbitResponse response = new InbitResponse
                {
                    inbit = currentInbit,
                    conveyor = request.conveyor
                };

                return JsonUtility.ToJson(response);
            }
        }
        catch (Exception e)
        {
            Debug.LogError($"Error processing request: {e.Message}");
        }

        return "{\"error\":\"Invalid request\"}";
    }
}

[Serializable]
public class OutbitRequest
{
    public string type;
    public string conveyor;
    public bool outbit;
}

[Serializable]
public class InbitResponse
{
    public bool inbit;
    public string conveyor;
}