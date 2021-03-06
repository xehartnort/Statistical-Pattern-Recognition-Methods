#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

template <class T>
ostream& operator<<(ostream& os, const vector<T> &v)
{
  for (int i = 0; i < v.size(); ++i)
  {
    os << "\t" << v[i];
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
    for(int i=0; i<v1.size(); i++)
    {
      result.push_back( v1[i]+v2[i] );
    }
  return result;
}
template <class T>
vector<T> operator-(const vector<T> &v1, const vector<T> &v2)
{
  vector<T> result;

    for(int i=0; i<v1.size(); i++)
    {
      result.push_back( v1[i]-v2[i] );
    }
  return result;
}
template <class T>
vector<T> operator*(const vector<T> &v1, const vector<T> &v2)
{
  vector<T> result;
    for(int i=0; i<v1.size(); i++)
    {
      result.push_back( v1[i]*v2[i] );
    }
  return result;
}
template <class T>
vector<T> operator/(const vector<T> &v1, const vector<T> &v2)
{
  vector<T> result;
    for(int i=0; i<v1.size(); i++)
    {
      result.push_back( v1[i]/v2[i] );
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
vector<T> multidimensionalMean( const vector<vector<T> > &points )
{
  int features = points[0].size();
  vector<T> mean( features, 0 );
  vector<T> points_size( features, points.size() );
  for(int i = 0; i < points.size(); i++)
  {
      mean =  mean + points[i]; 
  }
  mean = mean / points_size;
  return mean;
}
template <class T>
vector<T> multidimensionalStd( const vector<vector<T> > &points )
{
  int features = points[0].size();
  vector<T> mean = multidimensionalMean(points);
  vector<T> points_size( features, points.size() );
  vector<T> sum_squared( features, 0 );
  for (int i = 0; i < points.size(); ++i)
  {
    sum_squared = sum_squared + points[i]*points[i];
  }
  return sqrt( sum_squared/points_size - mean*mean );
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

class LinearClassifier
{
private:
  int classes;
  int features;
  vector<double> mean;
  vector<double> std;
  vector<int> train_objects_class;
  vector<vector<double> > train_objects;
  vector<vector<double> > gravity_points;
  vector<vector<double> > std_gravity_points;
  vector<vector<double> > weights;
  vector<vector<double> > std_weights;
  vector<double> std_residual_weights;
public:
  LinearClassifier(vector<vector<double> > train_objects, vector<int> train_objects_class, int classes, int features)
  {
    this->classes = classes;
    this->features = features;
    this->train_objects = train_objects;
    this->train_objects_class = train_objects_class;
    vector<vector<vector<double> > > tmp_class_points(classes+1); // assum class 0 is empty
    for (int i = 0; i < train_objects_class.size(); ++i)
    {
      tmp_class_points[ train_objects_class[i] ].push_back(train_objects[i]);
    }
    for(int i=1; i<classes+1; i++)
    {
      this->gravity_points.push_back( multidimensionalMean(tmp_class_points[i]) );
    }
    this->mean = multidimensionalMean(this->train_objects);
    this->std = multidimensionalStd(this->train_objects);
    vector<double> twos( features, 2.0 );
    for (int i = 0; i < classes; ++i)
    {
      this->std_gravity_points.push_back( (this->gravity_points[i]-mean)/std );
      this->weights.push_back( std_gravity_points[i]*twos );
      this->std_weights.push_back( std_gravity_points[i]*twos/std );
      this->std_residual_weights.push_back( -2*reduce(std_gravity_points[i]*mean/std) - reduce(std_gravity_points[i]*std_gravity_points[i]) );
    }
  }
  vector<vector<double> > getGravityPoints() const
  {
    return this->gravity_points;
  }
  vector<vector<double> > getWeights() const
  {
    return this->weights;
  }
  vector<vector<double> > getStandarizedGravityPoints() const
  {
    return this->std_gravity_points;
  }
  vector<vector<double> > getStandarizedWeights() const
  {
    return this->std_weights;
  }
  int classifyObject( const vector<double> &object ) const
  {
    vector<double> std_object = object;
    int classification=1;
    double current_value, max=reduce(std_weights[0]*std_object) + std_residual_weights[0];
    for(int i=0; i<classes; i++)
    {
      current_value = reduce(std_weights[i]*std_object) + std_residual_weights[i];
      if( current_value>=max )
      {
        classification = i+1;
        max=current_value;
      }
    }
    return classification;
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
  vector<vector<double> > train_objects, test_objects;
  vector<int> train_objects_real_class, test_objects_real_class;
  int classes=0, features=0, train_num_objects=0, test_num_objects=0;
  ifstream train_data (argv[1]);
  ifstream test_data (argv[2]);
  ofstream out_file (argv[3]);
  out_file << setprecision(3) << fixed;

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
      train_objects_real_class.push_back(values[0]);
      values.erase(values.begin()); 
      train_objects.push_back(values); //blablabla
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
      test_objects_real_class.push_back(values[0]);
      values.erase(values.begin()); 
      test_objects.push_back(values);
    }
    test_data.close();
  }
  else
  {
    cerr << "Unable to open " << argv[2];
    out_file.close();
    return -1;
  }

  LinearClassifier myClassifier(train_objects, train_objects_real_class, classes, features);
  out_file << "Class gravity centers before standardisation: " << endl;
  out_file << myClassifier.getGravityPoints() << endl;
  out_file << "Class gravity centers after standardisation: " << endl;
  out_file << myClassifier.getStandarizedGravityPoints() << endl; 
  out_file << "Weights without included standardisation: " << endl;
  out_file << myClassifier.getWeights() << endl;
  out_file << "Weights with included standardisation: " << endl;
  out_file << myClassifier.getStandarizedWeights() << endl;
  out_file << "Results of test:" << endl;
  out_file << "Obj.nr,\t\tTrue class,\t\tAssigned class" << endl;
  int classification, aciertos=0;
  vector<int> test_object_assign_class;
  vector<vector<int> > confusion_matrix(classes+1, vector<int>(classes+1, 0));
  vector<vector<double> > pconfusion_matrix(classes+1, vector<double>(classes+1, 0));
  vector<vector<double> > qconfusion_matrix(classes+1, vector<double>(classes+1, 0));
  for (int i = 0; i < test_objects.size(); ++i) //la conf matrix se puede rellenar aquí
  {
    test_object_assign_class.push_back( myClassifier.classifyObject(test_objects[i]) );
    if( test_object_assign_class[i]==test_objects_real_class[i] ){
      aciertos++;
    }
    confusion_matrix[test_objects_real_class[i]][test_object_assign_class[i]]++;
    out_file << "\t" << i << "\t\t" << test_objects_real_class[i] << "\t\t\t" << test_object_assign_class[i] << endl;
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

    // the following 6 lines are for pretty printing
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