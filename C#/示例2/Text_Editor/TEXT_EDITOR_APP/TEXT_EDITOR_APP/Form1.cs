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
    public partial class Form1 : Form
    {
        public string MyFileName { get; set; }
        public Form1()
        {
            InitializeComponent();
            openFileDialog1.InitialDirectory = @"C:\Users\weiping.sheng\Desktop\";
            openFileDialog1.Filter = "DOCXFILE | *.txt";
        }

        private void 打开ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                Form2 frm2 = new Form2();
                frm2.MdiParent = this;
                MyFileName = openFileDialog1.FileName;
                frm2.Text = Path.GetFileName(openFileDialog1.FileName);
                frm2.Show();
            }
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {

        }

        private void 水平排列ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.TileHorizontal);
        }

        private void 垂直排列ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.TileVertical);
        }

        private void 层叠排列ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.Cascade);
        }

        private void 最大处ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach(var item in this.MdiChildren)
            {
                item.WindowState = FormWindowState.Maximized;
            }
        }

        private void 最小处ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (var item in this.MdiChildren)
            {
                item.WindowState = FormWindowState.Minimized;
            }
        }

        private void 退出ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (var item in this.MdiChildren)
            {
                item.Close();
            }
        }

        private void 新建ToolStripMenuItem1_Click(object sender, EventArgs e)
        {
           
                Form3 frm3 = new Form3();
                frm3.MdiParent = this;
                frm3.Text = "未命名";
                frm3.Show();
        }
    }
}
