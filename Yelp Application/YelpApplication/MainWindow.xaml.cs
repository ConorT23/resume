using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Npgsql;

namespace milestone3app
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public static string current_u_id = "";
        public class Business
        {
            public string bid { get; set; }
            public string address { get; set; }
            public string name { get; set; }
            public string state { get; set; }
            public string city { get; set; }
            public double distance { get; set; }
            public double stars { get; set; }
            public int tips { get; set; }
            public int checkins { get; set; }

        }

        public class Friend
        {
            public string u_id { get; set; }
            public string name { get; set; }
            public int totalLikes { get; set; }
            public double avgStars { get; set; }
            public string yelpingSince { get; set; }

        }

        public class FriendTip
        {
            public string u_id { get; set; }
            public string username { get; set; }
            public string business { get; set; }
            public string city { get; set; }
            public string text { get; set; }
            public string date { get; set; }

        }

        public class businessFeature
        {
            public string type { get; set; }
            public string name { get; set; }
        }


        public MainWindow()
        {
            InitializeComponent();
            addState();
            addColumn2Grid();
            friendsListInit();
            friendsTipsListInit();
            sortByInit();
            


        }
        private string buildConnectionString()
        {
            // enter database and password  # "Host = localhost; Username = postgres; Database = milestone2db; password=blah3189"
            return "Host = localhost; Username = postgres; Database = milestone2db; password=blah3189";
        }
        private void addState()
        {

            using (var connection = new NpgsqlConnection(buildConnectionString()))
            {
                connection.Open();
                using (var cmd = new NpgsqlCommand())
                {

                    cmd.Connection = connection;
                    cmd.CommandText = "SELECT distinct business_state FROM business ORDER BY business_state";
                    try
                    {
                        var reader = cmd.ExecuteReader();
                        while (reader.Read())
                            stateList.Items.Add(reader.GetString(0));
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
        private void addColumn2Grid()
        {
            DataGridTextColumn col1 = new DataGridTextColumn();
            col1.Binding = new Binding("name");
            col1.Header = "BusinessName";
            col1.Width = 150;
            businessGrid.Columns.Add(col1);

            DataGridTextColumn col2 = new DataGridTextColumn();
            col2.Binding = new Binding("address");
            col2.Header = "Address";
            col2.Width = 150;
            businessGrid.Columns.Add(col2);

            DataGridTextColumn col3 = new DataGridTextColumn();
            col3.Binding = new Binding("city");
            col3.Header = "City";
            col3.Width = 50;
            businessGrid.Columns.Add(col3);

            DataGridTextColumn col4 = new DataGridTextColumn();
            col4.Binding = new Binding("state");
            col4.Header = "State";
            col4.Width = 25;
            businessGrid.Columns.Add(col4);

            DataGridTextColumn col5 = new DataGridTextColumn();
            col5.Binding = new Binding("distance");
            col5.Header = "Distance";
            col5.Width = 50;
            businessGrid.Columns.Add(col5);

            DataGridTextColumn col6 = new DataGridTextColumn();
            col6.Binding = new Binding("stars");
            col6.Header = "Stars";
            col6.Width = 50;
            businessGrid.Columns.Add(col6);

            DataGridTextColumn col7 = new DataGridTextColumn();
            col7.Binding = new Binding("tips");
            col7.Header = "# of tips";
            col7.Width = 50;
            businessGrid.Columns.Add(col7);

            DataGridTextColumn col8 = new DataGridTextColumn();
            col8.Binding = new Binding("checkins");
            col8.Header = "# of checkins";
            col8.Width = 60;
            businessGrid.Columns.Add(col8);

            DataGridTextColumn col9 = new DataGridTextColumn();
            col9.Binding = new Binding("bid");
            col9.Header = "";
            col9.Width = 0;
            businessGrid.Columns.Add(col9);
        }

        private void friendsListInit()
        {
            DataGridTextColumn col1 = new DataGridTextColumn();
            col1.Binding = new Binding("name");
            col1.Header = "Name";
            col1.Width = 100;
            friendsList.Columns.Add(col1);

            DataGridTextColumn col2 = new DataGridTextColumn();
            col2.Binding = new Binding("totalLikes");
            col2.Header = "Total likes";
            col2.Width = 45;
            friendsList.Columns.Add(col2);

            DataGridTextColumn col3 = new DataGridTextColumn();
            col3.Binding = new Binding("avgStars");
            col3.Header = "Avg Stars";
            col3.Width = 45;
            friendsList.Columns.Add(col3);

            DataGridTextColumn col4 = new DataGridTextColumn();
            col4.Binding = new Binding("yelpingSince");
            col4.Header = "Yelping Since";
            col4.Width = 150;
            friendsList.Columns.Add(col4);

            DataGridTextColumn col5 = new DataGridTextColumn();
            col5.Binding = new Binding("u_id");
            col5.Header = "";
            col5.Width = 0;
            friendsList.Columns.Add(col5);
        }

        private void friendsTipsListInit()
        {
            DataGridTextColumn col1 = new DataGridTextColumn();
            col1.Binding = new Binding("username");
            col1.Header = "User Name";
            col1.Width = 70;
            friendsTipsList.Columns.Add(col1);

            DataGridTextColumn col2 = new DataGridTextColumn();
            col2.Binding = new Binding("business");
            col2.Header = "Business";
            col2.Width = 70;
            friendsTipsList.Columns.Add(col2);

            DataGridTextColumn col3 = new DataGridTextColumn();
            col3.Binding = new Binding("city");
            col3.Header = "City";
            col3.Width = 50;
            friendsTipsList.Columns.Add(col3);

            DataGridTextColumn col4 = new DataGridTextColumn();
            col4.Binding = new Binding("text");
            col4.Header = "Text";
            col4.Width = 250;
            friendsTipsList.Columns.Add(col4);

            DataGridTextColumn col5 = new DataGridTextColumn();
            col5.Binding = new Binding("date");
            col5.Header = "Date";
            col5.Width = 80;
            friendsTipsList.Columns.Add(col5);
           
            DataGridTextColumn col6 = new DataGridTextColumn();
            col6.Binding = new Binding("u_id");
            col6.Header = "";
            col6.Width = 0;
            friendsTipsList.Columns.Add(col6);
        }

        private void sortByInit()
        {
            sortByBox.Items.Add("Name (Default)");
            sortByBox.Items.Add("Highest Rated");
            sortByBox.Items.Add("Most number of tips");
            sortByBox.Items.Add("Most checkins");
            sortByBox.Items.Add("Nearest");

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

        private void addCity(NpgsqlDataReader R)
        {
            cityList.Items.Add(R.GetString(0));
        }

        private void stateList_SelectionChanged_1(object sender, SelectionChangedEventArgs e)
        {
            cityList.Items.Clear();
            if (stateList.SelectedIndex > -1)
            {
                //string sqlStr = String.Format("SELECT distinct city FROM business WHERE state = '{0}' ORDER BY city;", stateList.SelectedItem.ToString());
                string sqlStr = $"SELECT distinct city FROM business WHERE business_state = '{stateList.SelectedItem.ToString()}' ORDER BY city;";
                executeQuery(sqlStr, addCity);
            }
        }
        private void addZipcode(NpgsqlDataReader R)
        {
            zipList.Items.Add(R.GetInt32(0));
        }

        private void cityList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            zipList.Items.Clear();
            if (cityList.SelectedIndex > -1)
            {
                string sqlStr = $"SELECT distinct zipcode FROM business WHERE business_state = '{stateList.SelectedItem.ToString()}' AND city = '{cityList.SelectedItem.ToString()}' ORDER BY zipcode;";
                executeQuery(sqlStr, addZipcode);
            }
        }
        public int businessQ = 0;
        private void addGridRow(NpgsqlDataReader R)
        {
            businessGrid.Items.Add(new Business() { name = R.GetString(0), address = R.GetString(1), city = R.GetString(2), state = R.GetString(3), stars = R.GetDouble(4), tips = R.GetInt32(5), checkins = R.GetInt32(6), bid = R.GetString(7) });
            businessQ++;
        }

        private void addCategory(NpgsqlDataReader R)
        {
            catList.Items.Add(R.GetString(0));
        }

        private void zipList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            catList.Items.Clear();
            if (zipList.SelectedIndex > -1)
            {
                string sqlStr2 = $"SELECT distinct category_name FROM categories, business WHERE categories.bid=business.bid AND business.zipcode = '{(int)zipList.SelectedItem}' ORDER BY category_name;";
                executeQuery(sqlStr2, addCategory);
            }
        }

        private void addBName(NpgsqlDataReader R)
        {
            selectedBusinessName.Text = R.GetString(0);
        }
        
        private void addBAddr(NpgsqlDataReader R)
        {
            selectedBusinessAddress.Text = R.GetString(0);
        }

        private void addBhours(NpgsqlDataReader R)
        {
            string temp = String.Format("Today({0}):    Opens: {1}0    Closes: {2}0", DateTime.Today.DayOfWeek, R.GetString(0), R.GetString(1));
            selectedBusinessHours.Text = temp;
        }

        //ATTEMPT AT OBTAINING BUSINESS FEATURES 
        //public List<businessFeature> items = new List<businessFeature>();
        //private void addBfeatures(NpgsqlDataReader R)
        //{
        //    featureList.Items.Add(new businessFeature() { type = "Categories", name = R.GetString(0) });
        //    //featureList.Items.Add(R.GetString(0));
        //    //items.Add(new businessFeature() { type = "Categories", name = R.GetString(0) });
        //    //featureList.ItemsSource = items;


        //}

        private void businessGrid_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            selectedBusinessName.Clear();
            selectedBusinessAddress.Clear();
            selectedBusinessHours.Clear();
            featureList.Items.Clear();
            
            if (businessGrid.SelectedIndex > -1)
            {
                Business B = businessGrid.Items[businessGrid.SelectedIndex] as Business;

                if ((B.bid != null) && (B.bid.ToString().CompareTo("") != 0))
                {
                    selectedBusinessName.Text = B.name;
                    selectedBusinessAddress.Text = B.address;
                    string sqlStr = $"SELECT t_open, t_close FROM Business_Hours WHERE bid = '{B.bid}' AND dayofweek = '{DateTime.Today.DayOfWeek}';";
                    executeQuery(sqlStr, addBhours);
                    //string sqlStr2 = $"SELECT category_name FROM Categories WHERE bid = '{B.bid}';";
                    //executeQuery(sqlStr2, addBfeatures);
                    
                }
            }
        }

        private void addButton_Click(object sender, RoutedEventArgs e)
        {
            if(catList.SelectedIndex > -1)
            {
                selectedCategoryList.Items.Add(catList.SelectedItem);
                update_category_list();
            }
        }

        private void removeButton_Click(object sender, RoutedEventArgs e)
        {
            if(selectedCategoryList.SelectedIndex > -1)
            {
                selectedCategoryList.Items.Remove(selectedCategoryList.SelectedItem);
                update_category_list();
            }
        }

        private void tipsButton_Click(object sender, RoutedEventArgs e)
        {
            if (businessGrid.SelectedIndex > -1)
            {
                Business B = businessGrid.Items[businessGrid.SelectedIndex] as Business;

                if ((B.bid != null) && (B.bid.ToString().CompareTo("") != 0))
                {
                    tipWindow tipWindow = new tipWindow(B.bid.ToString());
                    tipWindow.Show();
                }
            }
        }

        private void checkinButton_Click(object sender, RoutedEventArgs e)
        {
            if (businessGrid.SelectedIndex > -1)
            {
                Business B = businessGrid.Items[businessGrid.SelectedIndex] as Business;

                if ((B.bid != null) && (B.bid.ToString().CompareTo("") != 0))
                {
                    checkin checkin = new checkin(B.bid.ToString());
                    checkin.Show();
                }
            }
        }



        private void searchButton_Click(object sender, RoutedEventArgs e)
        {
            businessGrid.Items.Clear();
            businessQuantityText.Content = "";
            businessQ = 0;

            //BELOW QUERY NEEDS TO BE CHANGED - NEEDS TO INCLUDE ALL SELECTED CATEGORIES,ATTRIBUTES,PRICE,& MEAL PREFERENCES 
            if (zipList.SelectedIndex > -1)
            {
                int attr_used = 0;
                string sqlStr = $"SELECT Distinct business_name, business_address, city, business_state, stars, numTips, numCheckins, business.bid FROM business ";
                string sqlStr2 = "";
                if (selectedCategoryList.Items.Count > 0)
                {
                    //user inner join here as doing "FROM business, categories" or "FROM business, attributes" just about kills my pc's memory and I'm running 16 gigs
                    sqlStr += $"INNER JOIN categories ON business.bid=categories.bid ";
                    if (attribute_selected())//checks if any of the 'attr' or the 'meal' boxes are checked
                    {
                        sqlStr += $"INNER JOIN attributes ON business.bid=attributes.bid ";
                    }

                    //loop through all the categories needed
                    for (int i = 0; i < selectedCategoryList.Items.Count; i++)
                    {
                        sqlStr2 += $"AND category_name = '{selectedCategoryList.Items[i]}' ";
                    }

                    //if attribute selected loop through attributes (ps I'm sorry)
                    if (attribute_selected())
                    {
                        if (attr1.IsChecked.GetValueOrDefault()) // Accepts Credit Cards
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'BusinessAcceptsCreditCards' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                        if (attr2.IsChecked.GetValueOrDefault()) // Takes Reservations
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsReservations' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'RestaurantsReservations' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (attr3.IsChecked.GetValueOrDefault()) // Wheelchair Accessible
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'WheelchairAccessible' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'WheelchairAccessible' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (attr4.IsChecked.GetValueOrDefault()) // Outdoor Seating
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'OutdoorSeating' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'OutdoorSeating' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (attr5.IsChecked.GetValueOrDefault()) // Good for Kids
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'GoodForKids' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'GoodForKids' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (attr6.IsChecked.GetValueOrDefault()) // Good for Groups
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsGoodForGroups' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'RestaurantsGoodForGroups' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (attr7.IsChecked.GetValueOrDefault()) // Delivery
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsDelivery' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'RestaurantsDelivery' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (attr8.IsChecked.GetValueOrDefault()) // Take Out
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsTakeOut' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr += $"AND (attributes.attr_name = 'RestaurantsTakeOut' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (attr9.IsChecked.GetValueOrDefault()) // Free Wifi
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'WiFi' AND temp{attr_used}.a_value = 'free')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'WiFi' AND attributes.a_value = 'free' ";
                                attr_used++;
                            }
                        }
                        if (attr10.IsChecked.GetValueOrDefault()) // Bike Parking
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'BikeParking' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'BikeParking' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (meal1.IsChecked.GetValueOrDefault()) // Breakfast
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'breakfast' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'breakfast' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (meal2.IsChecked.GetValueOrDefault()) // Lunch
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'lunch' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'lunch' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (meal3.IsChecked.GetValueOrDefault()) // Brunch
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'brunch' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'brunch' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (meal4.IsChecked.GetValueOrDefault()) // Dinner
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'dinner' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'dinner' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (meal5.IsChecked.GetValueOrDefault()) // Dessert
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'dessert' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'dessert' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (meal6.IsChecked.GetValueOrDefault()) // Late Night
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'latenight' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'latenight' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (price2.IsChecked.GetValueOrDefault())
                        {
                            if (attr_used > 0)
                            {
                                sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsPriceRange2' AND temp{attr_used}.a_value = 'True')";
                                attr_used++;
                            }
                            else
                            {
                                sqlStr2 += $"AND (attributes.attr_name = 'RestaurantsPriceRange2' AND attributes.a_value = 'True' ";
                                attr_used++;
                            }
                        }
                        if (attr_used > 0)
                        {
                            sqlStr2 += $")";
                        }
                    }
                    for (int i = 1; i <= attr_used; i++)
                    {
                        sqlStr += $"INNER JOIN attributes as temp{i} ON business.bid=temp{i}.bid ";
                    }
                    sqlStr += $"WHERE zipcode = '{zipList.SelectedItem.ToString()}' ";

                    sqlStr += sqlStr2;

                }

                else if (attribute_selected())
                {

                    sqlStr += $"INNER JOIN attributes ON business.bid=attributes.bid ";

                    if (attr1.IsChecked.GetValueOrDefault()) // Accepts Credit Cards
                    {
                        sqlStr2 += $"AND (attributes.attr_name = 'BusinessAcceptsCreditCards' AND attributes.a_value = 'True' ";
                        attr_used++;
                    }
                    if (attr2.IsChecked.GetValueOrDefault()) // Takes Reservations
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsReservations' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'RestaurantsReservations' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (attr3.IsChecked.GetValueOrDefault()) // Wheelchair Accessible
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'WheelchairAccessible' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'WheelchairAccessible' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (attr4.IsChecked.GetValueOrDefault()) // Outdoor Seating
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'OutdoorSeating' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'OutdoorSeating' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (attr5.IsChecked.GetValueOrDefault()) // Good for Kids
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'GoodForKids' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'GoodForKids' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (attr6.IsChecked.GetValueOrDefault()) // Good for Groups
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsGoodForGroups' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'RestaurantsGoodForGroups' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (attr7.IsChecked.GetValueOrDefault()) // Delivery
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsDelivery' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'RestaurantsDelivery' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (attr8.IsChecked.GetValueOrDefault()) // Take Out
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsTakeOut' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr += $"AND (attributes.attr_name = 'RestaurantsTakeOut' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (attr9.IsChecked.GetValueOrDefault()) // Free Wifi
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'WiFi' AND temp{attr_used}.a_value = 'free')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'WiFi' AND attributes.a_value = 'free' ";
                            attr_used++;
                        }
                    }
                    if (attr10.IsChecked.GetValueOrDefault()) // Bike Parking
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'BikeParking' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'BikeParking' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (meal1.IsChecked.GetValueOrDefault()) // Breakfast
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'breakfast' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'breakfast' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (meal2.IsChecked.GetValueOrDefault()) // Lunch
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'lunch' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'lunch' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (meal3.IsChecked.GetValueOrDefault()) // Brunch
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'brunch' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'brunch' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (meal4.IsChecked.GetValueOrDefault()) // Dinner
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'dinner' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'dinner' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (meal5.IsChecked.GetValueOrDefault()) // Dessert
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'dessert' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'dessert' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (meal6.IsChecked.GetValueOrDefault()) // Late Night
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'latenight' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'latenight' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (price2.IsChecked.GetValueOrDefault())
                    {
                        if (attr_used > 0)
                        {
                            sqlStr2 += $"AND (temp{attr_used}.attr_name = 'RestaurantsPriceRange2' AND temp{attr_used}.a_value = 'True')";
                            attr_used++;
                        }
                        else
                        {
                            sqlStr2 += $"AND (attributes.attr_name = 'RestaurantsPriceRange2' AND attributes.a_value = 'True' ";
                            attr_used++;
                        }
                    }
                    if (attr_used > 0)
                    {
                        sqlStr2 += $")";
                    }

                    for (int i = 1; i <= attr_used; i++)
                    {
                        sqlStr += $"INNER JOIN attributes as temp{i} ON business.bid=temp{i}.bid ";
                    }
                    sqlStr += $"WHERE zipcode = '{zipList.SelectedItem.ToString()}' ";

                    sqlStr += sqlStr2;

                }

                if (!attribute_selected() && selectedCategoryList.Items.Count <= 0 )//and what ever else goes above)
                {
                    sqlStr = $"SELECT Distinct business_name, business_address, city, business_state, stars, numTips, numCheckins, business.bid FROM business " +
                    $"INNER JOIN categories ON business.bid=categories.bid ";
                    sqlStr += $"WHERE zipcode = '{zipList.SelectedItem.ToString()}' ";
                }

                sqlStr += $"ORDER BY business_name;";


                executeQuery(sqlStr, addGridRow);
                businessQuantityText.Content = businessQ; //update number of businesses
            }
        }

        public static void SortDataGrid(DataGrid dataGrid, int columnIndex = 0, System.ComponentModel.ListSortDirection sortDirection = System.ComponentModel.ListSortDirection.Ascending)
        {
            var column = dataGrid.Columns[columnIndex];

            // Clear current sort descriptions
            dataGrid.Items.SortDescriptions.Clear();

            // Add the new sort description
            dataGrid.Items.SortDescriptions.Add(new System.ComponentModel.SortDescription(column.SortMemberPath, sortDirection));

            // Apply sort
            foreach (var col in dataGrid.Columns)
            {
                col.SortDirection = null;
            }
            column.SortDirection = sortDirection;

            // Refresh items to display sort
            dataGrid.Items.Refresh();
        }

        private void sortByBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            int sortIndex = sortByBox.SelectedIndex;
            switch (sortIndex)
            {
                case 0:
                    SortDataGrid(businessGrid, 0);
                    break;
                case 1:
                    SortDataGrid(businessGrid, 5, System.ComponentModel.ListSortDirection.Descending);
                    break;
                case 2:
                    SortDataGrid(businessGrid, 6, System.ComponentModel.ListSortDirection.Descending);
                    break;
                case 3:
                    SortDataGrid(businessGrid, 7, System.ComponentModel.ListSortDirection.Descending);
                    break;
                case 4:
                    SortDataGrid(businessGrid, 4, System.ComponentModel.ListSortDirection.Descending);
                    break;
                default:
                    SortDataGrid(businessGrid, 0);
                    break;
            }
        }

        //*************************
        //*************************
        //                      ***
        //USER WINDOW FUNCTIONS ***
        //                      ***
        //*************************
        //*************************
        private void addIds(NpgsqlDataReader R)
        {
            useridList.Items.Add(R.GetString(0));
        }

        private void textBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            useridList.Items.Clear();
            if (textBox.Text != null)
            {
                string sqlStr = $"SELECT u_id FROM users WHERE u_name = '{textBox.Text}';";
                executeQuery(sqlStr, addIds);
            }
        }

        private void clearUserData()
        {
            nameText.Clear();
            starsText.Clear();
            fansText.Clear();
            yelpingSinceText.Clear();
            funnyVotesText.Clear();
            coolVotesText.Clear();
            usefulVotesText.Clear();
            tipCountText.Clear();
            tipLikesText.Clear();
        }

        private void addUserData(NpgsqlDataReader R)
        {
            nameText.Text = R.GetString(0);
            starsText.Text = R.GetDouble(1).ToString();
            fansText.Text = R.GetInt32(2).ToString();
            yelpingSinceText.Text = R.GetString(3);
            funnyVotesText.Text = R.GetInt32(4).ToString();
            coolVotesText.Text= R.GetInt32(5).ToString();
            usefulVotesText.Text = R.GetInt32(6).ToString();
            tipCountText.Text= R.GetInt32(7).ToString();
            tipLikesText.Text= R.GetInt32(8).ToString();
            

        }

        private void add2friendsList(NpgsqlDataReader R)
        {
            friendsList.Items.Add(new Friend() { u_id = R.GetString(0), name = R.GetString(1), totalLikes = R.GetInt32(2), avgStars = R.GetDouble(3), yelpingSince = R.GetString(4) });
        }

        private void add2friendsTipList(NpgsqlDataReader R)
        {   //date-4    business-1  CITY-2 text -3
            friendsTipsList.Items.Add(new FriendTip() { username = R.GetString(0), business = R.GetString(1), city = R.GetString(2), text = R.GetString(3), date = R.GetString(4) });
        }


        private void useridList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            clearUserData();
            friendsList.Items.Clear();
            friendsTipsList.Items.Clear();
            if(useridList.SelectedIndex > -1)
            {
                current_u_id = useridList.SelectedItem.ToString(); // global instance, to get in different window: MainWindow.current_u_id 
                string sqlStr = $"SELECT u_name, avg_stars, fans, yelping_since, funny, cool, useful, tipcount, totalLikes FROM users WHERE users.u_id = '{useridList.SelectedItem.ToString()}';";
                executeQuery(sqlStr, addUserData);
                string sqlStr2 = $"SELECT users.u_id, u_name, totalLikes, avg_stars, yelping_since FROM users,friend WHERE friend.u_id = '{useridList.SelectedItem.ToString()}' AND friend.friend_id = users.u_id;";
                executeQuery(sqlStr2, add2friendsList);

                //BELOW QUERY IS WRONG NEEDS TO BE LATEST TIP THAT EACH FRIEND HAS POSTED
                /*SELECT users.u_name, business.business_name, business.city, tip.tiptext, tip.tipdate
                 * FROM tip, friend, users, business
                 * WHERE friend.u_id='NXXC2iU0o3tDVu6YNaW3Ww' AND tip.u_id=friend.friend_id
                 *  AND friend.friend_id=users.u_id AND business.bid=tip.bid
                 *  AND tipdate in (SELECT MAX(tipdate)
                 *FROM tip, friend
                 *WHERE friend.u_id='NXXC2iU0o3tDVu6YNaW3Ww' AND tip.u_id=friend.friend_id
                 *GROUP BY friend.friend_id);
                 */
                string sqlStr3 = $"SELECT users.u_name, business.business_name, business.city, tip.tiptext, tip.tipdate " +
                    $"FROM tip, friend, users, business " +
                    $"WHERE friend.u_id = '{useridList.SelectedItem.ToString()}' AND tip.u_id = friend.friend_id " +
                    $"AND friend.friend_id=users.u_id AND business.bid=tip.bid " +
                    $"AND tipdate in (SELECT MAX(tipdate) " +
                    $"FROM tip, friend " +
                    $"WHERE friend.u_id='{useridList.SelectedItem.ToString()}' AND tip.u_id=friend.friend_id " +
                    $"GROUP BY friend.friend_id);";
                executeQuery(sqlStr3, add2friendsTipList);

            }
        }

        //updates the category list to filter out extra's after other categories have been selected
        private void update_category_list()
        {            
            if (selectedCategoryList.Items.Count > 0)
            {
                if (zipList.SelectedIndex > -1)
                {
                    
                    catList.Items.Clear();

                    //user inner join here as doing "FROM business, categories" or "FROM business, attributes" just about kills my pc's memory and I'm running 16 gigs
                    string sqlStr = $"SELECT distinct categories.category_name " +
                                    $"FROM (SELECT business.bid, category_name " +
                                        $"FROM business INNER JOIN categories ON business.bid = categories.bid " +
                                        $"WHERE zipcode = '{zipList.SelectedItem.ToString()}' AND (";

                    //loop through all the categories needed
                    for (int i = 0; i < selectedCategoryList.Items.Count; i++)
                    {
                        if (i == 0)
                        {
                            sqlStr += $"category_name = '{selectedCategoryList.Items[i]}'";
                        }
                        else
                        {
                            sqlStr += $" OR category_name = '{selectedCategoryList.Items[i]}'";
                        }
                    }

                    sqlStr += $") )as temp1 " +
                        $"INNER JOIN categories ON temp1.bid=categories.bid;";

                    executeQuery(sqlStr, addCategory);
                }
            }
            else
            {
                if (zipList.SelectedIndex > -1)
                {
                    catList.Items.Clear();
                    string sqlStr2 = $"SELECT distinct category_name FROM categories, business WHERE categories.bid=business.bid AND business.zipcode = '{(int)zipList.SelectedItem}' " +
                        $"ORDER BY category_name;";
                    executeQuery(sqlStr2, addCategory);
                }
            }
        }

        //determine if any of the attributes have been selected
        private bool attribute_selected()
        {
            if (attr1.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (attr2.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (attr3.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (attr4.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (attr5.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (attr6.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (attr7.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (attr8.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (attr9.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (attr10.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (meal1.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (meal2.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (meal3.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (meal4.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (meal5.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            if (meal6.IsChecked.GetValueOrDefault())
            {
                return true;
            }
            return false;
        }

        private void attr1_Checked(object sender, RoutedEventArgs e)
        {

        }

        private void price1_Checked(object sender, RoutedEventArgs e)
        {

        }

        private void updateButton_Click(object sender, RoutedEventArgs e)
        {
            
        }
    }
}
