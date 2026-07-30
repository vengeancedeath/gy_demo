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
    public partial class Form2 : Form
    {
        private string mytext;
        private int currentPage; //当前页
        private int totalPage; //总页数
        private int lengthPerPage; //每页字符数

        public Form2()
        {
            InitializeComponent();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            currentPage = 1;
            ReadText();
        }

        private void button5_Click(object sender, EventArgs e)
        {

        }

        private void Form2_Load(object sender, EventArgs e)
        {
            mytext = File.ReadAllText((this.MdiParent as Form1).MyFileName);
            //textBox1.Text = mytext;
            currentPage = 1;
            lengthPerPage = Convert.ToInt32(numericUpDown2.Value);
            totalPage = mytext.Length / lengthPerPage + 1;
            numericUpDown1.Maximum = totalPage;
            ReadText();
        }

        private void ReadText()
        {
            if (mytext.Length < lengthPerPage)
            {
                textBox1.Text = mytext;
                numericUpDown1.Value = 1;
            }
            else if (currentPage == totalPage)
            {
                textBox1.Text = mytext.Substring((totalPage - 1) * lengthPerPage);
                numericUpDown1.Value = totalPage;

            }
            else
            {
                int startIndex = (currentPage - 1) * lengthPerPage;
                textBox1.Text = mytext.Substring(startIndex, lengthPerPage);
                numericUpDown1.Value = currentPage;
            }
        }

        private void numericUpDown2_ValueChanged(object sender, EventArgs e)
        {
            lengthPerPage = Convert.ToInt32(numericUpDown2.Value);
            totalPage = mytext.Length / lengthPerPage + 1;
            numericUpDown1.Maximum = totalPage;
        }

        private void btnLastPage_Click(object sender, EventArgs e)
        {
            currentPage = totalPage;
            ReadText();
        }

        private void btnPrePage_Click(object sender, EventArgs e)
        {
            currentPage--;
            if (currentPage > 0)
            {
                ReadText();
            }
            else
            {
                currentPage = totalPage;
                MessageBox.Show("已经是第一页了！", "提示");
            }
        }

        private void btnNextPage_Click(object sender, EventArgs e)
        {
            currentPage++;
            if (currentPage <= totalPage)
            {
                ReadText();
            }
            else
            {
                currentPage = totalPage;
                MessageBox.Show("已经是最后一页了！", "提示");
            }
        }

        private void 文档背景色ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.BackColor = colorDialog1.Color;
            }
        }

        private void 文字颜色ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.ForeColor = colorDialog1.Color;
            }
        }

        private void 字体ToolStripMenuItem_Click(object sender, EventArgs e)
        {

            if (fontDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Font = fontDialog1.Font;
            }
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            currentPage = Convert.ToInt32(numericUpDown1.Value);
            ReadText();
        }
    }
}
