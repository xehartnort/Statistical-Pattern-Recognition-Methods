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
ostream& operator<<(ostream& os, const vector<vector<T> > &v)
{
  for (int i = 0; i < v.size(); ++i)
  {
    for (int j = 0; j < v[i].size(); ++j)
    {
      os << "\t" << v[i][j] << "\t";
    }  
    os << endl;
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
class kNN
{
private:
  unsigned int k;
  vector<pair <int, vector<double> > > std_train_objects;
  vector<double> mean; // for standarization purposes
  vector<double> std;  //
public:
  kNN(vector<pair<int, vector<double> > > train_objects, unsigned int k)
  {
    this->k = k;
    this->mean = multidimensionalMean(train_objects);
    this->std = multidimensionalStd(train_objects);
    for (auto it = train_objects.begin(); it != train_objects.end(); ++it)
    {
      this->std_train_objects.push_back( make_pair(it->first, (it->second - mean)/ std) );
    }
  }
  int classifyObject( const vector<double> &object ) const
  {
    vector<double> std_object = (object - mean)/std;
    vector< pair<int, vector<double> > > tmp = std_train_objects;
    sort(tmp.begin(), tmp.end(), 
      [&std_object](const pair<int,vector<double> > &p1, const pair<int,vector<double> > &p2)
      {
        return dist(p1.second, std_object) < dist(p2.second, std_object);
    });
    return tmp[k-1].first;
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
      train_objects.push_back( make_pair(values[0], vector<double> (values.begin()+1, values.end()) ) ); //blablabla
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
      test_objects.push_back( make_pair(values[0], vector<double> (values.begin()+1, values.end()) ) ); //blablabla
    }
    test_data.close();
  }
  else
  {
    cerr << "Unable to open " << argv[2];
    out_file.close();
    return -1;
  }
  kNN myClassifier(train_objects, 1);
  out_file << "Results of test:" << endl;
  out_file << "Obj.nr,\t\tTrue class,\t\tAssigned class" << endl;
  int classification, aciertos=0, i=0;
  vector<int> test_object_assign_class;
  vector<vector<int> > confusion_matrix(classes+1, vector<int>(classes+1, 0));
  vector<vector<double> > pconfusion_matrix(classes+1, vector<double>(classes+1, 0));
  vector<vector<double> > qconfusion_matrix(classes+1, vector<double>(classes+1, 0));
  for (int i = 0; i < test_objects.size(); ++i) //la conf matrix se puede rellenar aquí
  {
    test_object_assign_class.push_back( myClassifier.classifyObject(test_objects[i].second) );
    if( test_object_assign_class[i]==test_objects[i].first ){
      aciertos++;
    }
    confusion_matrix[test_objects[i].first][test_object_assign_class[i]]++;
    out_file << "\t" << i << "\t\t\t\t" << test_objects[i].first << "\t\t\t\t" << test_object_assign_class[i] << endl;
  }
  for (int i=1; i < classes+1; i++)
  {
    double sum1=0, sum2=0;
    for (int j=1; j < classes+1; ++j)
    {
      sum1 += confusion_matrix[i][j];
      sum2 += confusion_matrix[j][i];
    }
    
    for (int j=1; j < classes+1; ++j)
    {
      pconfusion_matrix[i][j]  = 100*confusion_matrix[i][j]/(double)sum1;
      qconfusion_matrix[i][j]  = 100*confusion_matrix[j][i]/(double)sum2;
    }

    // the following 6 lines are for pretty print
    confusion_matrix[0][i]=i;
    confusion_matrix[i][0]=i;
    pconfusion_matrix[0][i]=i;
    pconfusion_matrix[i][0]=i;
    qconfusion_matrix[0][i]=i;
    qconfusion_matrix[i][0]=i;
  }
  out_file << endl << "Error rate:\t" << 100*(test_objects.size()-aciertos)/(double)test_objects.size() << "%" << endl;
  out_file << endl << "Confusion matrix R: " << endl << confusion_matrix << endl;
  out_file << endl << "Confusion matrix P: " << endl << pconfusion_matrix << endl;
  out_file << endl << "Confidence matrix Q: " << endl << qconfusion_matrix << endl;
  // hacer el ouput más bonito
  out_file.close();
  return 0;
}