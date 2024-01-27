import React, { useEffect, useState } from "react";
import { View, Text, TextInput, Button } from "react-native";
import { Client, Message } from "react-native-mqtt";

const App = () => {
  const [mqttClient, setMqttClient] = useState(null);
  const [messageFan, setMessageFan] = useState("");
  const [messageFeed, setMessageFeed] = useState("");
  const [receivedMessageTemp, setReceivedMessageTemp] = useState("");
  const [receivedMessagePee, setReceivedMessagePee] = useState("");

  useEffect(() => {
    const client = new Client({
      uri: "34.123.169.68",
      clientId: "clientId-" + Math.random().toString(16).substr(2, 8),
    });
    setMqttClient(client);

    return () => {
      if (client) {
        client.disconnect();
      }
    };
  }, []);

  const connectToBroker = async () => {
    try {
      await mqttClient.connect();
      console.log("Connected to MQTT broker");
    } catch (error) {
      console.error("Error connecting to MQTT broker", error);
    }
  };

  const subscribeToTopic = async () => {
    try {
      await mqttClient.subscribe("your_topic");
      console.log("Subscribed to topic: your_topic");
    } catch (error) {
      console.error("Error subscribing to topic", error);
    }
  };

  const sendMessageFan = async () => {
    try {
      const messageObject = new Message(message);
      await mqttClient.send("smart-cage/fan", messageObject);
      console.log("Message sent:", message);
    } catch (error) {
      console.error("Error sending message", error);
    }
  };

  const sendMessageFeed = async () => {
    try {
      const messageObject = new Message(message);
      await mqttClient.send("smart-cage/feeder", messageObject);
      console.log("Message sent:", message);
    } catch (error) {
      console.error("Error sending message", error);
    }
  };

  useEffect(() => {
    if (mqttClient) {
      mqttClient.on("messageReceived", (message) => {
        console.log("Message received:", message);
        setReceivedMessage(message.data);
      });
    }
  }, [mqttClient]);

  return (
    <View style={{ flex: 1, justifyContent: "center", alignItems: "center" }}>
      <Text>MQTT React Native Example</Text>
      <TextInput
        style={{
          height: 40,
          borderColor: "gray",
          borderWidth: 1,
          margin: 10,
          padding: 5,
        }}
        placeholder="Enter message"
        onChangeText={(text) => setMessage(text)}
        value={message}
      />
      <Button title="Connect to Broker" onPress={connectToBroker} />
      <Button title="Subscribe to Topic" onPress={subscribeToTopic} />
      <Button title="On Fan" onPress={sendMessageFan} />
      <Button title="Feed Pet" onPress={sendMessageFeed} />
      <Text>Temperature: {receivedMessageTemperature}</Text>
      <Text>Pet Pee: {receivedMessagePee}</Text>
    </View>
  );
};

export default App;
