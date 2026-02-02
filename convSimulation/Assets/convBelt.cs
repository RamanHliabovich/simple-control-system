using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class convBelt : MonoBehaviour
{
    public float speed;
    public Vector3 direction;
    public List<GameObject> onBelt;

    [Header("Spawn Settings")]
    public GameObject cubePrefab;
    public Vector3 spawnPosition;
    public bool useWorldPosition = true;
    public bool spawnOnStart = true;
    public bool destroyOnExit = true;

    [Header("TCP Integration")]
    public string conveyorId = "conv101";
    public int tcpPort = 5001;
    public bool enableTcp = true;

    private static TcpServer tcpServer; // Static so it's shared across all conveyors

    void Awake()
    {
        if (enableTcp && tcpServer == null)
        {
            tcpServer = new TcpServer(tcpPort);
            tcpServer.Start();
        }
    }

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        // Spawn a cube at the start
        if (spawnOnStart && cubePrefab != null)
        {
            SpawnCube();
        }
    }

    // Update is called once per frame
    void Update()
    {
        // Only move belt if outbit is true
        bool outbitActive = tcpServer != null && tcpServer.GetOutbitStatus(conveyorId);
        
        if (outbitActive)
        {
            for (int i = 0; i < onBelt.Count; i++)
            {
                onBelt[i].GetComponent<Rigidbody>().linearVelocity = speed * direction;
            }
        }

        // Update inbit status based on belt occupancy
        if (tcpServer != null)
        {
            bool hasObjects = onBelt.Count > 0;
            tcpServer.SetInbitStatus(conveyorId, hasObjects);
        }
    }

    private void OnCollisionEnter(Collision collision)
    {
        onBelt.Add(collision.gameObject);
    }

    private void OnCollisionExit(Collision collision)
    {
        onBelt.Remove(collision.gameObject);
        
        // Destroy the cube that exited
        if (destroyOnExit)
        {
            Destroy(collision.gameObject);
        }
        
        // Spawn a new cube when object exits
        SpawnCube();
    }

    private void SpawnCube()
    {
        if (cubePrefab != null)
        {
            Vector3 finalSpawnPosition = useWorldPosition ? spawnPosition : transform.position + spawnPosition;
            Instantiate(cubePrefab, finalSpawnPosition, Quaternion.identity);
        }
    }

    void OnApplicationQuit()
    {
        if (tcpServer != null)
        {
            tcpServer.Stop();
        }
    }
}
