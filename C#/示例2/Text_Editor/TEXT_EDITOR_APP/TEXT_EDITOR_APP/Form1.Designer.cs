namespace TEXT_EDITOR_APP
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.文件ToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.新建ToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.打开ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.退出ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.窗口ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.水平排列ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.垂直排列ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.层叠排列ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.最大处ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.最小处ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog1_FileOk);
            // 
            // menuStrip1
            // 
            this.menuStrip1.GripMargin = new System.Windows.Forms.Padding(2, 2, 0, 2);
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.文件ToolStripMenuItem1,
            this.窗口ToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(800, 32);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // 文件ToolStripMenuItem1
            // 
            this.文件ToolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.新建ToolStripMenuItem1,
            this.打开ToolStripMenuItem,
            this.toolStripSeparator1,
            this.退出ToolStripMenuItem});
            this.文件ToolStripMenuItem1.Name = "文件ToolStripMenuItem1";
            this.文件ToolStripMenuItem1.Size = new System.Drawing.Size(62, 28);
            this.文件ToolStripMenuItem1.Text = "文件";
            // 
            // 新建ToolStripMenuItem1
            // 
            this.新建ToolStripMenuItem1.Name = "新建ToolStripMenuItem1";
            this.新建ToolStripMenuItem1.Size = new System.Drawing.Size(270, 34);
            this.新建ToolStripMenuItem1.Text = "新建";
            this.新建ToolStripMenuItem1.Click += new System.EventHandler(this.新建ToolStripMenuItem1_Click);
            // 
            // 打开ToolStripMenuItem
            // 
            this.打开ToolStripMenuItem.Name = "打开ToolStripMenuItem";
            this.打开ToolStripMenuItem.Size = new System.Drawing.Size(270, 34);
            this.打开ToolStripMenuItem.Text = "打开";
            this.打开ToolStripMenuItem.Click += new System.EventHandler(this.打开ToolStripMenuItem_Click);
            // 
            // 退出ToolStripMenuItem
            // 
            this.退出ToolStripMenuItem.Name = "退出ToolStripMenuItem";
            this.退出ToolStripMenuItem.Size = new System.Drawing.Size(270, 34);
            this.退出ToolStripMenuItem.Text = "退出";
            this.退出ToolStripMenuItem.Click += new System.EventHandler(this.退出ToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(267, 6);
            // 
            // 窗口ToolStripMenuItem
            // 
            this.窗口ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.水平排列ToolStripMenuItem,
            this.垂直排列ToolStripMenuItem,
            this.层叠排列ToolStripMenuItem,
            this.toolStripSeparator2,
            this.最大处ToolStripMenuItem,
            this.最小处ToolStripMenuItem});
            this.窗口ToolStripMenuItem.Name = "窗口ToolStripMenuItem";
            this.窗口ToolStripMenuItem.Size = new System.Drawing.Size(62, 28);
            this.窗口ToolStripMenuItem.Text = "窗口";
            // 
            // 水平排列ToolStripMenuItem
            // 
            this.水平排列ToolStripMenuItem.Name = "水平排列ToolStripMenuItem";
            this.水平排列ToolStripMenuItem.Size = new System.Drawing.Size(270, 34);
            this.水平排列ToolStripMenuItem.Text = "水平排列";
            this.水平排列ToolStripMenuItem.Click += new System.EventHandler(this.水平排列ToolStripMenuItem_Click);
            // 
            // 垂直排列ToolStripMenuItem
            // 
            this.垂直排列ToolStripMenuItem.Name = "垂直排列ToolStripMenuItem";
            this.垂直排列ToolStripMenuItem.Size = new System.Drawing.Size(270, 34);
            this.垂直排列ToolStripMenuItem.Text = "垂直排列";
            this.垂直排列ToolStripMenuItem.Click += new System.EventHandler(this.垂直排列ToolStripMenuItem_Click);
            // 
            // 层叠排列ToolStripMenuItem
            // 
            this.层叠排列ToolStripMenuItem.Name = "层叠排列ToolStripMenuItem";
            this.层叠排列ToolStripMenuItem.Size = new System.Drawing.Size(270, 34);
            this.层叠排列ToolStripMenuItem.Text = "层叠排列";
            this.层叠排列ToolStripMenuItem.Click += new System.EventHandler(this.层叠排列ToolStripMenuItem_Click);
            // 
            // 最大处ToolStripMenuItem
            // 
            this.最大处ToolStripMenuItem.Name = "最大处ToolStripMenuItem";
            this.最大处ToolStripMenuItem.Size = new System.Drawing.Size(270, 34);
            this.最大处ToolStripMenuItem.Text = "最大处";
            this.最大处ToolStripMenuItem.Click += new System.EventHandler(this.最大处ToolStripMenuItem_Click);
            // 
            // 最小处ToolStripMenuItem
            // 
            this.最小处ToolStripMenuItem.Name = "最小处ToolStripMenuItem";
            this.最小处ToolStripMenuItem.Size = new System.Drawing.Size(270, 34);
            this.最小处ToolStripMenuItem.Text = "最小处";
            this.最小处ToolStripMenuItem.Click += new System.EventHandler(this.最小处ToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(267, 6);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.menuStrip1);
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Form1";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem 文件ToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem 新建ToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem 打开ToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem 退出ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 窗口ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 水平排列ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 垂直排列ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 层叠排列ToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem 最大处ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 最小处ToolStripMenuItem;
    }
}

