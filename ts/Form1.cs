using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Net;
using System.Reflection.Emit;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ts
{
    public partial class Form1 : Form
    {
        int startflag = 0;
        int flag_sensor;
        string RxString;
        string temp, voice = "0";
        char charb = 'B';
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
                serialPort1.Close();

            serialPort1.PortName = "COM5";
            serialPort1.BaudRate = 115200;

        }


        private void Serial_Start_Click(object sender, EventArgs e)
        {
            serialPort1.PortName = "COM5";
            serialPort1.BaudRate = 115200;
            serialPort1.Open();
            if (serialPort1.IsOpen)
            {
                textBox1.ReadOnly = false;
            }
        }

        private void Read_From_TS_Click(object sender, EventArgs e)
        {
            WebClient client = new WebClient();

            label1.Text = client.DownloadString("http://api.thingspeak.com/channels/1964028/field/field1/last.text");//use your channel id

        }

        private void SerialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {

            Console.WriteLine("Data Received");
            RxString = serialPort1.ReadExisting();

            if (RxString.Contains(charb))
            {
                startflag = 9;
            }
            else { }

            if (startflag == 9)
            {
                startflag++;
            }

            else if (startflag == 10)
            {
                temp = RxString;
                startflag++;
            }
            else if (startflag == 11)
            {
                voice = RxString;
                startflag++;
            }

            else
            {
                // startflag = 0;
            }

            this.Invoke(new EventHandler(Current_Data_Click));

        }


        private void Serial_Stop_Click(object sender, EventArgs e)
        {
            serialPort1.Close();

            textBox1.ReadOnly = true;
        }

        private void Current_Data_Click(object sender, EventArgs e)
        {
            textBox1.AppendText(RxString);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (!string.Equals(textBox1.Text, ""))
            {
                if (serialPort1.IsOpen) serialPort1.Close();
                try
                {
                    if (RxString[0] == 'B')
                    {
                        flag_sensor = 10;
                    }

                    String[] spearator = { "B" };
                    Int32 count = 3;

                    // using the method
                    String[] strlist = RxString.Split(spearator, count,
                           StringSplitOptions.RemoveEmptyEntries);

                    const string WRITEKEY = "V6OGNIHZXALOW6SR"; ////use your channel API keys
                    string strUpdateBase = "http://api.thingspeak.com/update";
                    string strUpdateURI = strUpdateBase + "?key=" + WRITEKEY;
                    //string strField1 = textBox1.Text;
                    //string strField1 = temp;
                    //string strField2 = voice;
                    //string strField2 = motion;
                    HttpWebRequest ThingsSpeakReq;
                    HttpWebResponse ThingsSpeakResp;


                   
                    strUpdateURI += "&field1=" + strlist[0];

                    strUpdateURI += "&field2=" + strlist[1];

                    strUpdateURI += "&field3=" + strlist[2];
                    
                    
                    ThingsSpeakReq = (HttpWebRequest)WebRequest.Create

(strUpdateURI);

                    ThingsSpeakResp = (HttpWebResponse)

ThingsSpeakReq.GetResponse();
                    ThingsSpeakResp.Close();

                    if (!(string.Equals(ThingsSpeakResp.StatusDescription,

"OK")))
                    {
                        Exception exData = new Exception

(ThingsSpeakResp.StatusDescription);
                        throw exData;
                    }

                }
                catch (Exception ex)
                {

                }
                textBox1.Text = "";
                serialPort1.Open();
            }

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
