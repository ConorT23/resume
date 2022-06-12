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
    /// Interaction logic for checkin.xaml
    /// </summary>
    public partial class checkin : Window
    {
        private string bid = "";
        public checkin(string bid)
        {
            InitializeComponent();
            this.bid = String.Copy(bid);
            addColumn2Grid();
            loadCheckins();
        }

        public class Checkin
        {
            public string month { get; set; }
            public int num { get; set; }


        }

        private void addColumn2Grid()
        {
            DataGridTextColumn col1 = new DataGridTextColumn();
            col1.Binding = new Binding("month");
            col1.Header = "Month";
            col1.Width = 390;
            businessCheckins.Columns.Add(col1);

            DataGridTextColumn col2 = new DataGridTextColumn();
            col2.Binding = new Binding("num");
            col2.Header = "Number of Checkins";
            col2.Width = 390;
            businessCheckins.Columns.Add(col2);

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

        private int i = 0;
        private void add2checkins(NpgsqlDataReader R)
        {
            string month = "";
            switch (i)
            {
                case 1:
                    month = "January";
                    break;
                case 2:
                    month = "February";
                    break;
                case 3:
                    month = "March";
                    break;
                case 4:
                    month = "April";
                    break;
                case 5:
                    month = "May";
                    break;
                case 6:
                    month = "June";
                    break;
                case 7:
                    month = "July";
                    break;
                case 8:
                    month = "August";
                    break;
                case 9:
                    month = "September";
                    break;
                case 10:
                    month = "October";
                    break;
                case 11:
                    month = "November";
                    break;
                case 12:
                    month = "December";
                    break;
                default:
                    month = "None";
                    break;
            }
            businessCheckins.Items.Add(new Checkin() { num = R.GetInt32(0), month = month });
        }


        private void loadCheckins()
        {
            businessCheckins.Items.Clear();
            for (i = 1; i < 13; i++)
            {
                string sqlStr = $"SELECT COUNT(c_time) FROM checkins,business WHERE business.bid = '{this.bid}' AND checkins.bid = business.bid AND c_month = '{i}' ;";
                executeQuery(sqlStr, add2checkins);
            }


        }
    }
}
