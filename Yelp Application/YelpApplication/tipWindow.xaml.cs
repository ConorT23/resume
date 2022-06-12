using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Npgsql;

namespace milestone3app
{
    /// <summary>
    /// Interaction logic for tipWindow.xaml
    /// </summary>
    public partial class tipWindow : Window
    {
        public class Tip
        {
            public string date { get; set; }
            public string username { get; set; }
            public int likes { get; set; }
            public string tip_text { get; set; }
            public string u_id { get; set; }

        }

        public class Friend_review
        {
            public string u_id { get; set; }
            public string name { get; set; }
            public string date { get; set; }
            public string text { get; set; }

        }

        private string bid = "";
        public tipWindow(string bid)
        {
            InitializeComponent();
            this.bid = String.Copy(bid);
            addColumn2Grid();
            loadTipDetails();
            friendReviewListInit();
            loadFriendTips();
        }

        private void addColumn2Grid()
        {
            DataGridTextColumn col1 = new DataGridTextColumn();
            col1.Binding = new Binding("date");
            col1.Header = "Date";
            col1.Width = 150;
            businessTips.Columns.Add(col1);

            DataGridTextColumn col2 = new DataGridTextColumn();
            col2.Binding = new Binding("username");
            col2.Header = "User Name";
            col2.Width = 80;
            businessTips.Columns.Add(col2);

            DataGridTextColumn col3 = new DataGridTextColumn();
            col3.Binding = new Binding("likes");
            col3.Header = "Likes";
            col3.Width = 50;
            businessTips.Columns.Add(col3);

            DataGridTextColumn col4 = new DataGridTextColumn();
            col4.Binding = new Binding("tip_text");
            col4.Header = "Text";
            col4.Width = 457;
            businessTips.Columns.Add(col4);

            DataGridTextColumn col5 = new DataGridTextColumn();
            col5.Binding = new Binding("u_id");
            col5.Header = "";
            col5.Width = 0;
            businessTips.Columns.Add(col5);

        }

        private void friendReviewListInit()
        {
            DataGridTextColumn col1 = new DataGridTextColumn();
            col1.Binding = new Binding("name");
            col1.Header = "Name";
            col1.Width = 80;
            friendReviews.Columns.Add(col1);

            DataGridTextColumn col2 = new DataGridTextColumn();
            col2.Binding = new Binding("date");
            col2.Header = "Date";
            col2.Width = 150;
            friendReviews.Columns.Add(col2);

            DataGridTextColumn col3 = new DataGridTextColumn();
            col3.Binding = new Binding("text");
            col3.Header = "Text";
            col3.Width = 507;
            friendReviews.Columns.Add(col3);

            DataGridTextColumn col4 = new DataGridTextColumn();
            col4.Binding = new Binding("u_id");
            col4.Header = "";
            col4.Width = 0;
            friendReviews.Columns.Add(col4);
        }
        private string buildConnectionString()
        {
            return "Host = localhost; Username = postgres; Database = milestone2db; password=blah3189";
        }

        private void executeQuery(string sqlstr, Action<NpgsqlDataReader> myf)
        {
            using (var connection = new NpgsqlConnection(buildConnectionString()))
            {
                connection.Open();
                using (var cmd = new NpgsqlCommand())
                {

                    cmd.Connection = connection;
                    cmd.CommandText = sqlstr;
                    try
                    {
                        var reader = cmd.ExecuteReader();
                        while (reader.Read())
                            myf(reader);
                    }
                    catch (NpgsqlException ex)
                    {
                        Console.WriteLine(ex.Message.ToString());
                        System.Windows.MessageBox.Show("SQL Error - " + ex.Message.ToString());
                    }
                    finally
                    {
                        connection.Close();
                    }
                }
            }
        }
        private void addTipsText(NpgsqlDataReader R)
        {
            businessTips.Items.Add(new Tip() { u_id = R.GetString(0), username = R.GetString(1), tip_text = R.GetString(2), date = R.GetString(3), likes = R.GetInt32(4)});
        }

        private void addFriendTips(NpgsqlDataReader R)
        {
            friendReviews.Items.Add(new Friend_review() { u_id = R.GetString(0), name = R.GetString(1), date = R.GetString(2), text = R.GetString(3) });
        }


        //NOT WORKING IDK WHY - program jumps passed query without executing
        private void loadFriendTips()
        {
            if (MainWindow.current_u_id != "")
            {
                friendReviews.Items.Clear();
                string sqlStr = $"SELECT users.u_id, u_name, tipDate, tipText FROM tip,friend,users WHERE friend.u_id = '{MainWindow.current_u_id}' AND users.u_id = friend.friend_id AND tip.u_id = friend.friend_id AND tip.bid = '{this.bid}';";
                executeQuery(sqlStr, addFriendTips);
            }
        }

        private void loadTipDetails()
        {
            businessTips.Items.Clear();
            string sqlStr = $"SELECT tip.u_id, users.u_name, tip.tiptext, tipDate, likes FROM tip, business, users WHERE business.bid = '{this.bid}' AND business.bid = tip.bid AND tip.u_id = users.u_id ORDER BY tipDate DESC;";
            executeQuery(sqlStr, addTipsText);
        }

        private void refreshBusinessTips(NpgsqlDataReader R)
        {
            //do nothing
        }
        
        private void likeTipButton_Click(object sender, RoutedEventArgs e)
        {
            if(businessTips.SelectedIndex > -1)
            {
                Tip t = businessTips.Items[businessTips.SelectedIndex] as Tip;
                if ((t.u_id != null) && (t.u_id.ToString().CompareTo("") != 0))
                {
                    string sqlStr = $"UPDATE tip SET likes = likes + 1 WHERE tip.u_id = '{t.u_id}' AND tip.bid = '{this.bid}' AND tip.tipDate = '{t.date}';";
                    executeQuery(sqlStr, refreshBusinessTips);
                    loadTipDetails();
                }

                    
            }
        }
        private void addTipText(NpgsqlDataReader R)
        {
            //do nothing
        }

        //MAY NEED TO CHANGE DATE FORMAT 
        private void addTipButton_Click(object sender, RoutedEventArgs e)
        {
            if(MainWindow.current_u_id != "")
            {
                string newText = tipText.Text;
                string sqlStr = $"INSERT INTO tip (bid, u_id, tipdate, likes, tiptext) VAlUES ('{this.bid}', '{MainWindow.current_u_id}', '{DateTime.Now.ToString()}', '0', '{newText}');"; //need to update with actual values
                executeQuery(sqlStr, addTipText);
                tipText.Clear();
                loadTipDetails();
            }
            else
            {
                MessageBox.Show("Error: Must be logged in", "Alert", MessageBoxButton.OK);
            }

        }
    }
}
