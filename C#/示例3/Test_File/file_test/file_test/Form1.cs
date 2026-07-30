using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace file_test
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "DOCU | *.txt";
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                StreamReader streamReader = new StreamReader(openFileDialog1.FileName);
                textBox1.Text = streamReader.ReadToEnd();
                streamReader.Close();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string filename;
            saveFileDialog1.Filter = "DOCU | *.txt";
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                filename = saveFileDialog1.FileName;
                StreamWriter streamWriter = new StreamWriter(filename);
                streamWriter.Write(textBox1.Text);
                streamWriter.WriteLine("");
                streamWriter.Close();
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox4_TextChanged(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
            //二进制
            //if (openFileDialog1.ShowDialog() == DialogResult.OK)
            //{
            //    FileStream fsRW1 = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read);
            //    BinaryReader binaryReader = new BinaryReader(fsRW1);
            //    var firstInt = binaryReader.ReadInt32();
            //    var secondInt = binaryReader.ReadDouble();
            //    var thirdInt = binaryReader.ReadString();
            //    textBox2.Text = firstInt.ToString();
            //    textBox3.Text = secondInt.ToString();
            //    textBox4.Text = thirdInt;
            //    binaryReader.Close();
            //    fsRW1.Close();
            //}

            //去序列化
            Mydata mydata2 = new Mydata();

            FileStream fileStream = new FileStream("C:\\Users\\weiping.sheng\\Desktop\\file_text_xlh.txt", FileMode.Open, FileAccess.Read);
            BinaryFormatter binaryFormatter = new BinaryFormatter();
            mydata2 = (Mydata)binaryFormatter.Deserialize(fileStream);

            textBox2.Text = mydata2.first.ToString();
            textBox3.Text = mydata2.second.ToString();
            textBox4.Text = mydata2.str;
            fileStream.Close();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            //二进制
            //saveFileDialog1.Filter = "DOCU | *.txt";
            //if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            //{
            //    FileStream fsRW = new FileStream(saveFileDialog1.FileName, FileMode.Create, FileAccess.Write);
            //    BinaryWriter bw = new BinaryWriter(fsRW);
            //    var firstInt = Convert.ToInt32(textBox2.Text);
            //    var secondDouble = Convert.ToDouble(textBox3.Text);
            //    bw.Write(firstInt);
            //    bw.Write(secondDouble);
            //    bw.Write(textBox4.Text);
            //    bw.Close();
            //    fsRW.Close();
            //}

            //序列化
            Mydata mydata = new Mydata();
            mydata.first = Convert.ToInt32(textBox2.Text);
            mydata.second = Convert.ToDouble(textBox3.Text);
            mydata.str = textBox4.Text;

            FileStream fileStream = new FileStream("C:\\Users\\weiping.sheng\\Desktop\\file_text_xlh.txt", FileMode.Create, FileAccess.Write);
            BinaryFormatter binaryFormatter = new BinaryFormatter();
            binaryFormatter.Serialize(fileStream, mydata);
            fileStream.Close();


        }

        //序列化
        [Serializable]
        public class Mydata
        {
            public int first;
            public double second;
            public string str;
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            foreach (var item in Directory.GetLogicalDrives())
            {
                comboBox1.Items.Add(item);
            }
        }

        private string[] dirs;
        private string[] files;
        
        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            dirs = Directory.GetDirectories(comboBox1.Text);
            files = Directory.GetFiles(comboBox1.Text);
            listBox1.Items.Clear();
            foreach(var item in dirs)
            {
                listBox1.Items.Add(Path.GetFileNameWithoutExtension(item));
            }
            listBox2.Items.Clear();
            foreach (var item in files)
            {
                listBox2.Items.Add(Path.GetFileName(item));
            }

        }

        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            var currentdir = dirs[listBox1.SelectedIndex];
            dirs = Directory.GetDirectories(currentdir);
            listBox1.Items.Clear();
            foreach (var item in dirs)
            {
                listBox1.Items.Add(Path.GetFileNameWithoutExtension(item));
            }
            listBox2.Items.Clear();
            foreach (var item in Directory.GetFiles(currentdir))
            {
                listBox2.Items.Add(Path.GetFileName(item));
            }
        }
    }
}
