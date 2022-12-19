namespace ts
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.Serial_Start = new System.Windows.Forms.Button();
            this.Serial_Stop = new System.Windows.Forms.Button();
            this.Read_From_TS = new System.Windows.Forms.Button();
            this.Current_Data = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // Serial_Start
            // 
            this.Serial_Start.Location = new System.Drawing.Point(78, 56);
            this.Serial_Start.Name = "Serial_Start";
            this.Serial_Start.Size = new System.Drawing.Size(164, 44);
            this.Serial_Start.TabIndex = 0;
            this.Serial_Start.Text = "Serial_Start";
            this.Serial_Start.UseVisualStyleBackColor = true;
            this.Serial_Start.Click += new System.EventHandler(this.Serial_Start_Click);
            // 
            // Serial_Stop
            // 
            this.Serial_Stop.Location = new System.Drawing.Point(425, 56);
            this.Serial_Stop.Name = "Serial_Stop";
            this.Serial_Stop.Size = new System.Drawing.Size(164, 44);
            this.Serial_Stop.TabIndex = 1;
            this.Serial_Stop.Text = "Serial_Stop";
            this.Serial_Stop.UseVisualStyleBackColor = true;
            this.Serial_Stop.Click += new System.EventHandler(this.Serial_Stop_Click);
            // 
            // Read_From_TS
            // 
            this.Read_From_TS.Location = new System.Drawing.Point(78, 153);
            this.Read_From_TS.Name = "Read_From_TS";
            this.Read_From_TS.Size = new System.Drawing.Size(164, 44);
            this.Read_From_TS.TabIndex = 2;
            this.Read_From_TS.Text = "Read_From_TS";
            this.Read_From_TS.UseVisualStyleBackColor = true;
            this.Read_From_TS.Click += new System.EventHandler(this.Read_From_TS_Click);
            // 
            // Current_Data
            // 
            this.Current_Data.AutoSize = true;
            this.Current_Data.Location = new System.Drawing.Point(137, 259);
            this.Current_Data.Name = "Current_Data";
            this.Current_Data.Size = new System.Drawing.Size(106, 20);
            this.Current_Data.TabIndex = 3;
            this.Current_Data.Text = "Current_Data";
            this.Current_Data.Click += new System.EventHandler(this.Current_Data_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(411, 171);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(178, 26);
            this.textBox1.TabIndex = 4;
            this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            // 
            // serialPort1
            // 
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.SerialPort1_DataReceived);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(462, 259);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(51, 20);
            this.label1.TabIndex = 5;
            this.label1.Text = "label1";
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.Current_Data);
            this.Controls.Add(this.Read_From_TS);
            this.Controls.Add(this.Serial_Stop);
            this.Controls.Add(this.Serial_Start);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button Serial_Start;
        private System.Windows.Forms.Button Serial_Stop;
        private System.Windows.Forms.Button Read_From_TS;
        private System.Windows.Forms.Label Current_Data;
        private System.Windows.Forms.TextBox textBox1;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label label1;
    }
}

