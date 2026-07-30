using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void 文件ToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void 退出ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void 复制ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            textBox1.Copy();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void 剪切ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            textBox1.Cut();
        }

        private void 粘贴ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            textBox1.Paste();
        }

        private void 撤销ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            textBox1.Undo();
        }

        private void 操作ToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
        {
            if (textBox1.SelectedText == string.Empty)
            {
                剪切ToolStripMenuItem.Enabled = false;
            }
            else
            {
                剪切ToolStripMenuItem.Enabled = true;
            }
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {
            if (textBox1.SelectedText == string.Empty)
            {
                剪切ToolStripMenuItem1.Enabled = false;
            }
            else
            {
                剪切ToolStripMenuItem1.Enabled = true;
            }
        }
    }
}
