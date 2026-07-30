using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TEXT_EDITOR_APP
{
    public partial class Form3 : Form
    {
        public Form3()
        {
            InitializeComponent();
            saveFileDialog1.InitialDirectory = @"C:\Users\weiping.sheng\Desktop\";
            saveFileDialog1.Filter = "DOCXFILE | *.txt";
        }

        private void 编辑ToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void 保存ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                File.WriteAllText(saveFileDialog1.FileName, textBox1.Text);
                this.Text = saveFileDialog1.FileName;   
            }


        }
    }
}
