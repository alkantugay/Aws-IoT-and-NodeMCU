## Aws-IoT-and-NodeMCU

![Getting-Started-with-Amazon-AWS-IoT-and-ESP8266](https://user-images.githubusercontent.com/51509025/63457087-f51d1200-c458-11e9-9abf-0bee19830a9c.jpg)
## Basic steps on AWS sided:

Create an amazon account to log on AWS Console

1- Go to the Amazon Web Services home page. (https://aws.amazon.com)		 
 				
2- Create a new account as personal or professional.

3- Add a payment method (It's only will cost you up to 3 USD for development and academic purposes)

4- Log in into AWS console with your credentials.


Access IoT Core on the AWS Console

In order to make your first IoT device in AWS services, you will have to go to:

![Ekran Alıntısı](https://user-images.githubusercontent.com/51509025/63459578-75457680-c45d-11e9-98d3-71ddf0f89f6b.JPG)

As some basic things has to be updated than follow my next guide that will help you how to create a Thing, and what are the ARN, EndPoint, and shadow on IoT Core console.

Create a Thing on IoT Core Console

Now that you have accessed the IoT Core console, on the sidebar go to Manage -> Things, and on the right corner just hit create.


![2](https://user-images.githubusercontent.com/51509025/63459655-9908bc80-c45d-11e9-9f93-070738eb74f8.JPG)

After this select the option Create a Single Thing, and as we are doing this guide only for learning purposes, just give a name to the device, and don't change any other option, and even don't create a certificate at this time.



So, after the device is created you can go into device's options, and among a lot of options here, we will have two of them that it's important for us at this time.

- Shadow - A JSON document used to store and retrieve current state information for a device. Summarizing it's MQTT topics that you can publish messages in JSON format.

- Interact - On this option you will able to see the REST endpoint to communicate with your device, and some default MQTT topics for your shadow.

Take note of the device endpoint and the update shadow topic, like the next image:

![3](https://user-images.githubusercontent.com/51509025/63459794-dff6b200-c45d-11e9-86e7-7bda8668d537.JPG)

## IAM - Authorizathion for IoT Services

In Amazon web services, the IAM (Identity Access Management) is responsible for controlling, and creating users of your web services. Besides, in this console you can set roles and policies to restrict or give authorization of what your users can use or not. These users are not the users of your applications, but users that will access your web services.

For example, imagine that you have a team of developers, whom are developing a mobile application in the cloud. Thus, every developer will have an user on your console with their respective authorities.

So, If don't have any user let's create an user for you with administrator roles, remembering that is a good practice creating a user for you, because your root user of your web services has to be used only in extreme cases. So, now search for IAM on your AWS Console.


![0](https://user-images.githubusercontent.com/51509025/63460094-8478f400-c45e-11e9-88e6-f35a3a62148f.png)

On the IAM sidebar, go to Users -> Add user. Put your username and select Programmatic access as an access type.

After that, attach existing policies to your user. In this guide we will give administrator access to the user, but you can give the user the police AWSIoTFullAccess, therefore this user will be only able to use IoT Core services.


![0 (1)](https://user-images.githubusercontent.com/51509025/63460170-a83c3a00-c45e-11e9-8bd6-a8c1125b4fab.png)

After the user is created, you will be able to download the Access key ID, and Secret Access Key. Get the .csv file with the key/secret that we will use on the ESP8266 code. 

Now that the device(Thing) in the AWS IoT Core console is created, you have to note some AWS information that was retrieved in the last steps. They are:

- AWS-Endpoint - The endpoint of your created Thing, and where all communications will be established.

- AWS-KEY / AWS-Secret - This key/secret is generated when you create a new user on IAM(Identity Access Management) console.

- AWS-Region - Your Thing was created in some of the AWS availability zones, please note where you've created it.

These information will be used in the ESP8266 code to handle you connect to your created Thing.
In the IAM section where you can create a key pair with the key and secret for an user, it is a good practice creating a new user (and grant just IoT services permission). Avoid use the key/secret key of your main AWS console user. 
Now that the AWS section was covered, let's move to the ESP8266 section.


## Basic steps on ESP8266(NodeMCU) Side

Download Arduino IDE and Prepare it to upload codes in your ESP8266

In this tutorial, we are using the arduino IDE to develop and upload the code into the ESP8266, so first of all download the arduino IDE with the link:

https://www.arduino.cc/en/main/software


With the IDE installed, go to preferences on the File menu, and paste the url in the additional boards manager URL's: 

*http://arduino.esp8266.com/stable/package_esp8266com_index.json*

This will download the board manger to the development kits with the ESP chip, and your screen should look like this:


![0 (2)](https://user-images.githubusercontent.com/51509025/63460907-23eab680-c460-11e9-8d1f-25f934efb49f.png)



Now go to Tools-> Board-> Board Manager and search for esp8266 installing the esp8266 by ESP8266 Community, like the next image:


![0 (3)](https://user-images.githubusercontent.com/51509025/63460956-3b29a400-c460-11e9-9491-23299267fbda.png)
(*Install 2.4 version*)

If you have the NodeMCU development kit like the one in this tutorial first image, you will have to select your development kit to point out it to your IDE. # You can go to: Tools-> Board-> <Your-development-kit-name>. 
If you have the NodeMCU your screen should look like this:
  

![0 (4)](https://user-images.githubusercontent.com/51509025/63461104-9bb8e100-c460-11e9-8868-78a158bbde4a.png)

After that we are ready to go with the code, I will send the complete code here with a few comments on each part to explain how does it work, but summarizing the execution of the code:

1- Your device will try to connect on your wifi access point by the credentials you provided

2- After connected to internet, the AWS parameters will be set with the region, endpoint, key and secret you have provided.

3- After all parameters set, the connect() function will be called in order to connect to AWS server, and if it's successful it will subscribe a MQTT topic and publish a message in the topic. You can specify your shadow topic to first test the connection between IoT Core and your device.

4-So remember to change the AWS parameters on the initial part of the code


## Notes 
The mqtt code and necessary libraries are in file 
