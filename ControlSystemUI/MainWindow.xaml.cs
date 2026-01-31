using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Windows;

namespace ControlSystemUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private const string ServerHost = "127.0.0.1";
        private const int ServerPort = 5000;

        public MainWindow()
        {
            InitializeComponent();
        }

        private async void StartButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                StatusText.Text = "Sending start command...";
                var command = new { action = "start", conveyor = "conv101" }; // todo hardocded conveyor name
                string json = JsonSerializer.Serialize(command);
                string response = await SendSocketMessage(json);
                StatusText.Text = response;
            }
            catch (Exception ex)
            {
                StatusText.Text = $"Error: {ex.Message}";
            }
        }

        private async void StopButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                StatusText.Text = "Sending stop command...";
                var command = new { action = "stop", conveyor = "conv101" }; // todo hardcoded conveyor name
                string json = JsonSerializer.Serialize(command);
                string response = await SendSocketMessage(json);
                StatusText.Text = response;
            }
            catch (Exception ex)
            {
                StatusText.Text = $"Error: {ex.Message}";
            }
        }

        private async Task<string> SendSocketMessage(string message)
        {
            using (TcpClient client = new TcpClient())
            {
                await client.ConnectAsync(ServerHost, ServerPort);

                using (NetworkStream stream = client.GetStream())
                {
                    // Send the message
                    byte[] data = Encoding.UTF8.GetBytes(message);
                    await stream.WriteAsync(data, 0, data.Length);
                    await stream.FlushAsync();

                    // Read the response
                    byte[] buffer = new byte[1024];
                    int bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length);
                    string responseJson = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                    // Parse and return the response
                    return ParseResponse(responseJson);
                }
            }
        }

        private string ParseResponse(string responseJson)
        {
            try
            {
                using (JsonDocument doc = JsonDocument.Parse(responseJson))
                {
                    JsonElement root = doc.RootElement;

                    if (root.TryGetProperty("status", out JsonElement statusElement))
                    {
                        string status = statusElement.GetString();
                        return status == "success" 
                            ? "Start command executed successfully!" 
                            : "Command failed - check server logs";
                    }

                    if (root.TryGetProperty("error", out JsonElement errorElement))
                    {
                        return $"Server error: {errorElement.GetString()}";
                    }

                    return "Unknown response from server";
                }
            }
            catch (Exception ex)
            {
                return $"Failed to parse response: {ex.Message}";
            }
        }
    }
}