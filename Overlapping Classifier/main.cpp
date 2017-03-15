#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <utility>

using namespace std;

template <class T>
ostream& operator<<(ostream& os, const vector<T> &v)
{
  for (int i = 0; i < v.size(); ++i)
  {
    os << "\t" << v[i] << "\t";
  }
  return os;
}
template <class T>
ostream& operator<<(ostream& os, const vector<vector<T> > &v)
{
  for (int i = 0; i < v.size(); ++i)
  {
    os << v[i] << endl;
  }
  return os;
}
template <class T>
vector<T> operator+(const vector<T> &v1, const vector<T> &v2)
{
  vector<T> result;
  if ( v1.size()!=v2.size() )
  {
    result = v1;
  }
  else
  {
    for(int i=0; i<v1.size(); i++)
    {
      result.push_back( v1[i]+v2[i] );
    }
  }
  return result;
}
template <class T>
vector<T> operator-(const vector<T> &v1, const vector<T> &v2)
{
  vector<T> result;
  if ( v1.size()!=v2.size() )
  {
    result = v1;
  }
  else
  {
    for(int i=0; i<v1.size(); i++)
    {
      result.push_back( v1[i]-v2[i] );
    }
  }
  return result;
}
template <class T>
vector<T> operator*(const vector<T> &v1, const vector<T> &v2)
{
  vector<T> result;
  if ( v1.size()!=v2.size() )
  {
    result = v1;
  }
  else
  {
    for(int i=0; i<v1.size(); i++)
    {
      result.push_back( v1[i]*v2[i] );
    }
  }
  return result;
}
template <class T>
vector<T> operator/(const vector<T> &v1, const vector<T> &v2)
{
  vector<T> result;
  if ( v1.size()!=v2.size() )
  {
    result = v1;
  }
  else
  {
    for(int i=0; i<v1.size(); i++)
    {
      result.push_back( v1[i]/v2[i] );
    }
  }
  return result;
}
template <class T>
vector<T> sqrt(const vector<T> &v1)
{
  vector<T> result( v1.size() );
  for (int i = 0; i < v1.size(); ++i)
  {
    result[i] = sqrt(v1[i]);
  }
  return result;
}
template <class T>
vector<T> multidimensionalMean( const vector<pair<int, vector<T> > > &points )
{
  int features = points.begin()->second.size();
  vector<T> mean( features, 0 );
  vector<T> points_size( features, points.size() );
  for (auto it = points.begin(); it != points.end(); ++it)
  {
      mean =  mean + it->second; 
  }
  mean = mean / points_size;
  return mean;
}
template <class T>
vector<T> multidimensionalStd( const vector<pair<int, vector<T> > > &points )
{
  int features = points.begin()->second.size();
  vector<T> mean = multidimensionalMean(points);
  vector<T> points_size( features, points.size() );
  vector<T> sum_squared( features, 0 );
  for (auto it = points.begin(); it != points.end(); ++it)
  {
    sum_squared = sum_squared + (it->second - mean)*(it->second - mean);
  }
  return sqrt(sum_squared/points_size);
}  
template <class T>
T reduce(const vector<T> & point)
{
  T sum=0;
  for (int i = 0; i < point.size(); ++i)
  {
    sum += point[i];
  }
  return sum;
}
template <class T>
double dist(const vector<T> &v1, const vector<T> &v2)
{
  return sqrt(reduce( (v1-v2)*(v1-v2) ));
}
class OverlappingClassifier
{
private:
  vector<double> std;  //
  vector<double> mean; // for standarization purposes
  unsigned int classes;
  vector<double> max_radius;
  vector<pair <int, vector<double> > > std_train_objects;
public:
  OverlappingClassifier(vector<pair<int, vector<double> > > train_objects, unsigned int classes)
  {
    this->classes = classes;
    this->mean = multidimensionalMean(train_objects);
    this->std = multidimensionalStd(train_objects);
    vector<vector<vector<double> > > tmp_class_points(classes+1); // assum class 0 is empty
    // this->std_train_objects = train_objects;
    vector<double> tmp;
    for (const pair<int, vector<double> > &it : train_objects)
    {
      tmp = (it.second - mean)/std;
      this->std_train_objects.push_back( make_pair(it.first, tmp) );
      tmp_class_points[ it.first ].push_back( tmp );
      // tmp_class_points[ it.first ].push_back( it.second );
    }
    double max, tmp_dist;
    this->max_radius.resize(classes+1); // class 0 is assumed empty
    for (int i = 1; i < classes+1; ++i)  // class 0 is assumed empty
    {
      max = 0;
      for(int j = 0; j < tmp_class_points[i].size(); ++j)
      {
        for (int k = 0; k < j; ++k)
        {
          tmp_dist = dist(tmp_class_points[i][j], tmp_class_points[i][k]);
          // cout << "class " << i << " " << tmp_dist << endl;
          if(tmp_dist >= max)
          {
            max = tmp_dist;
          }
        }
      }
      max_radius[i] = max;
    }
  }
  vector<bool> classifyObject( const vector<double> &object ) const
  {
    vector<bool> classifications(this->classes+1, false);  // class 0 is assumed empty
    vector<double> std_object = (object - mean)/std;
    // vector<double> std_object = object;
    // cout << "\t\t NEW CLASSIFICATION \t\t" << endl;
    // cout << "object " << object << endl;
    // cout << "std_object " << std_object << endl;
    // cout << "radius " << max_radius << endl;
    for(const pair<int, vector<double> > &a : std_train_objects)
    {
      for(int i=1; i<classes+1; i++)  // class 0 is assumed empty
      {
        if( a.first==i and dist(std_object, a.second) < max_radius[i] )
        {
          classifications[i]=true;
        }
      }
    }
    return classifications;
  }
};

vector<double> stringToDoubleVector(const string &line, char sep=' ')
{
  vector<double> result;
  string number;
  for(int i=0; i<line.size(); i++)
  {
    if( line[i] != sep )
    {
      number+=line[i];  
    }
    else
    {
      if( !number.empty() )
      {
        result.push_back( stod(number) );
        number.clear();
      }
    }
  }
  if( !number.empty() )
  {
    result.push_back( stod(number) );
  }
  return result;
}

int main (int argc, char ** argv) 
{
  if(argc < 4){
    cerr << "You must supply three arguments: " << endl;
    cerr << "\t1. Path to a file with the train set" << endl;
    cerr << "\t2. Path to a file with the test set" << endl;
    cerr << "\t3. Path to the output file" << endl;
    cerr << "Example: ./main.out train_set.txt test_set.txt output_file.txt" << endl;
    return -1;
  }
  string line;
  vector<double> values;
  vector<pair<int, vector<double> > > train_objects, test_objects;
  int classes=0, features=0, train_num_objects=0, test_num_objects=0;
  ifstream train_data (argv[1]);
  ifstream test_data (argv[2]);
  ofstream out_file (argv[3]);
  out_file << setprecision(4) << fixed;

  if ( train_data.is_open() )
  {
    getline (train_data,line);
    values = stringToDoubleVector(line);
    classes = values[0];
    features = values[1];
    train_num_objects = values[2];
    for(int i=0; i<train_num_objects; i++)
    {
      getline (train_data,line);
      values = stringToDoubleVector(line);
      train_objects.push_back( make_pair(values[0], vector<double> (values.begin()+1, values.end()) ) );
    }
    train_data.close();
  }
  else
  {
    cerr << "Unable to open " << argv[1];
    out_file.close();
    return -1;
  }
  
  if ( test_data.is_open() )
  {
    getline (test_data,line);
    values = stringToDoubleVector(line);
    if( values[0] != classes or values[1] != features)
    {
      cerr << "File "<< argv[2] <<" is malformed, wrong number of classes or features" << endl;
      return -1;
    }
    test_num_objects = values[2];
    for(int i=0; i<test_num_objects; i++)
    {
      getline (test_data,line);
      values = stringToDoubleVector(line);
      test_objects.push_back( make_pair(values[0], vector<double> (values.begin()+1, values.end()) ) );
    }
    test_data.close();
  }
  else
  {
    cerr << "Unable to open " << argv[2];
    out_file.close();
    return -1;
  }
  OverlappingClassifier myClassifier(train_objects, 3);
  out_file << "OBJECT CLASS MEMBERSHIP" << endl;
  out_file << "Obj.nr,\t\tTrue class,\tA1,\tA2,\tA3" << endl;
  vector<bool> classif;
  for (int i = 0; i < test_objects.size(); ++i) //la conf matrix se puede rellenar aquí
  {
    classif = myClassifier.classifyObject(test_objects[i].second);
    out_file << "\t" << i << "\t\t" << test_objects[i].first << "\t" << classif[1] << "\t" << classif[2] << "\t" << classif[3] << endl;
  }
  out_file.close();
  return 0;
}