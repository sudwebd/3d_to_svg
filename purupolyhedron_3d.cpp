// #define WINDOWS  /* uncomment this line to use it for windows.*/
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <fstream>
#include <math.h>
#include <array>
#include "Eigen/Dense"

#define PI 3.14159265358979323846

using namespace Eigen;
using namespace std;

unsigned IMG_WIDTH = 10000;
unsigned IMG_HEIGHT = 10000;
const string PARALLEL = "parallel";

class Light{
private:
	Vector3d position;
	Vector3d ambient;
	Vector3d diffuse;
	Vector3d specular;

public:
	Light(){
		position = Vector3d(0,0,1);
		ambient = Vector3d(1.0,1.0,1.0);
		diffuse = Vector3d(0.8,0.8,0.8);
		specular = Vector3d(0.5,0.5,0.5);
	}

	Vector3d get_position(){
		return position;
	}

	void set_position(Vector3d position){
		this->position = position;
	}

	Vector3d get_ambient(){
		return ambient;
	}

	void set_ambient(Vector3d ambient){
		this->ambient = ambient;
	}

	Vector3d get_diffuse(){
		return diffuse;
	}

	void set_diffuse(Vector3d diffuse){
		this->diffuse = diffuse;
	}

	Vector3d get_specular(){
		return specular;
	}

	void set_specular(Vector3d specular){
		this->specular = specular;
	}
};

class Material{
private:
	string name;
	Vector3d ambient;
	Vector3d diffuse;
	Vector3d specular;
	double shininess;
	double opacity;
	int illum_no;

public:
	Material(){
		this->name = "";
		this-> ambient = Vector3d(0.2,0.2,0.2);
		this-> diffuse = Vector3d(1,0,0);
		this-> specular = Vector3d(0.3,0.3,0.3);
		this->shininess = 0;
		this->opacity = 1.0;
		this-> illum_no = 0;
	}

	Material(string name, Vector3d ambient, Vector3d diffuse,
		Vector3d specular, double shininess, double opacity, int illum_no){
		this->name = name;
		this-> ambient = ambient;
		this-> diffuse = diffuse;
		this-> specular = specular;
		this->shininess = shininess;
		this->opacity = opacity;
		this-> illum_no = illum_no;
	}

	string get_name(){
		return this->name;
	}

	void set_name(string name){
		this->name = name;
	}

	Vector3d get_ambient(){
		return this->ambient;
	}

	void set_ambient(Vector3d ambient){
		this->ambient = ambient;
	}

	Vector3d get_diffuse(){
		return this->diffuse;
	}

	void set_diffuse(Vector3d diffuse){
		this->diffuse = diffuse;
	}

	Vector3d get_specular(){
		return this->specular;
	}

	void set_specular(Vector3d specular){
		this->specular = specular;
	}

	double get_shininess(){
		return this->shininess;
	}

	void set_shininess(double shininess){
		this->shininess = shininess;
	}

	double get_opacity(){
		return this->opacity;
	}

	void set_opacity(double opacity){
		this->opacity = opacity;
	}

	int get_illum_no(){
		return this->illum_no;
	}

	void set_illum_no(int illum_no){
		this->illum_no = illum_no;
	}

};

class Object_3D{
private:
	vector< Vector3d > vertices;
	vector< vector<int> > edges;
	vector< vector<int> > faces;
	map< vector<int>,string> material_of_faces;
	map<string,Material> materials;
	string name;
	string type;

public:
	Object_3D(){

	}

	vector<Vector3d> getVertices(){
		return vertices;
	}

	vector< vector<int> > getEdges(){
		return edges;
	}

	vector< vector<int> > getFaces(){
		return faces;
	}

	string getType(){
		return type;
	}

	map< vector<int>,string> getMaterialOfFaces(){
		return material_of_faces;
	}

	map<string,Material> getMaterials(){
		return materials;
	}

	Material getMaterialFromName(string name){
		return materials[name];
	}

	void addMaterial(string name, Material material){
		this->materials[name] = material;
	}

	void addMaterialOfFaces(vector<int> face, string name){
		this->material_of_faces[face] = name;
	}

	void addVertex(Vector3d point){
		vertices.push_back(point);
	}

	void addEdge(vector<int> edge){
		edges.push_back(edge);
	}

	void addFace(vector<int> face){
		faces.push_back(face);
	}

	void setName(string name){
		this->name = name;
	}

	void setType(string type){
		if(type=="-f" || type == "--face"){
			if (this->faces.size() != 0){
				this->type = "face";
			}
			else{
				try{
					throw "Face list is empty!";
				}
				catch(char const* e){
					cout<<"An error occurred: "<< e <<endl;
				}
			}
		}

		if(type=="-e" || type == "--edge"){
			if (this->edges.size() != 0){
				this->type = "edge";
			}
			else{
				try{
					throw "Edge list is empty!";
				}
				catch(char const* e){
					cout<<"An error occurred: "<< e <<endl;
				}
			}
		}
	}

};

void parse_material(Object_3D& obj, string filename, string current_dir){
	string line;
	filename = current_dir+filename;
	ifstream file (filename.c_str());
	if(file.is_open()){
		int count_newmtl = 0;
		Material material;
		while(getline(file,line)){
			istringstream iss(line);
			vector<string> tokens;
			copy(istream_iterator<string>(iss),
			     istream_iterator<string>(),
			     back_inserter(tokens));
			if(tokens.size()>0){
				if(tokens[0][0]=='#');
				else if(tokens[0] == "newmtl"){
					if(count_newmtl>0){
						obj.addMaterial(material.get_name(), material);
					}
					material = Material();
					material.set_name(tokens[1]);
					count_newmtl++;
				}
				else if(tokens[0] == "Ka"){
					Vector3d ambient;
					ambient(0) = strtod(tokens[1].c_str(),NULL);
					ambient(1) = strtod(tokens[2].c_str(),NULL);
					ambient(2) = strtod(tokens[3].c_str(),NULL);
					material.set_ambient(ambient);
				}
				else if(tokens[0] == "Kd"){
					Vector3d diffuse;
					diffuse(0) = strtod(tokens[1].c_str(),NULL);
					diffuse(1) = strtod(tokens[2].c_str(),NULL);
					diffuse(2) = strtod(tokens[3].c_str(),NULL);
					material.set_diffuse(diffuse);
				}
				else if(tokens[0] == "Ks"){
					Vector3d specular;
					specular(0) = strtod(tokens[1].c_str(),NULL);
					specular(1) = strtod(tokens[2].c_str(),NULL);
					specular(2) = strtod(tokens[3].c_str(),NULL);
					material.set_specular(specular);
				}
				else if(tokens[0] == "Ns"){
					double shininess = strtod(tokens[1].c_str(),NULL);
					material.set_shininess(shininess);
				}
				else if(tokens[0] == "d"){
					double opacity = strtod(tokens[1].c_str(),NULL);
					material.set_opacity(opacity);
				}
				else if(tokens[0] == "illum"){
					int illum_no = atoi(tokens[1].c_str());
					material.set_illum_no(illum_no);
				}
			}
		}
		if(count_newmtl>0){
			obj.addMaterial(material.get_name(), material);
		}
	}
	else{

	}
}

bool parse_object(string filename, Object_3D& obj, string current_dir){
	string line;
	ifstream file (filename.c_str());
	if(file.is_open()){
		Material material;
		while(getline(file,line)){
			istringstream iss(line);
			vector<string> tokens;
			copy(istream_iterator<string>(iss),
			     istream_iterator<string>(),
			     back_inserter(tokens));
			if(tokens.size()>0){
				Vector3d vertex;
				vector<int> edge;
				vector<int> face;
				if(tokens[0][0]=='#');
				else if(tokens[0]=="v"){
					vertex(0) = strtod(tokens[1].c_str(),NULL);
					vertex(1) = strtod(tokens[2].c_str(),NULL);
					vertex(2) = strtod(tokens[3].c_str(),NULL);
					obj.addVertex(vertex);
				}
				else if(tokens[0]=="l"){
					for(int i=1;i<tokens.size();i++){
						edge.push_back(atoi(tokens[i].c_str()));
					}
					obj.addEdge(edge);
				}
				else if(tokens[0]=="f"){
					for(int i=1;i<tokens.size();i++){
						int vertex_no = atoi(tokens[i].c_str());
						face.push_back(vertex_no);
					}
					obj.addFace(face);
					obj.addMaterialOfFaces(face,material.get_name());
				}
				else if(tokens[0]=="mtllib"){
					string file_name = tokens[1];
					parse_material(obj,file_name,current_dir);
				}
				else if(tokens[0]=="usemtl"){
					string name = tokens[1];
					material = obj.getMaterialFromName(name);
				}
			}
		}
		file.close();
		return true;
	}
	else {
		try{
			throw "Unable to open file ";
		}
		catch(char const* e){
			cout<<e<<filename<<endl;
		}
		return false;
	}
}

string get_current_directory(string filename) {
	char buff[FILENAME_MAX];
	GetCurrentDir( buff, FILENAME_MAX );
	string current_working_dir(buff);
	current_working_dir+='/';
	int index = filename.length()-1;
	while(index>=0){
		if(filename[index]=='/')
			break;
		index--;
	}
	for(int i=0;i<=index;i++){
		current_working_dir+=filename[i];
	}
	return current_working_dir;
}


string get_filename(string str){
	string filename = "";
	int index = str.length()-1;
	while(index>=0){
		if(str[index]=='/')
			break;
		index--;
	}
	index++;
	for(;str[index]!='.';index++)
		filename += str[index];
	//cout<<filename<<endl;
	return (filename);
}

vector< pair<string,double> > get_rotations(char* argv[]){
	/*cout<<"Enter rotation axes and angles: "<<endl;
	string rotation_string = "";
	getline(cin,rotation_string);

	istringstream iss(rotation_string);
	vector<string> tokens;
	copy(istream_iterator<string>(iss),
	     istream_iterator<string>(),
	     back_inserter(tokens));
	vector< pair<string,double> > rotations;
	if(tokens.size()>0){
		for(int i=0;i<tokens.size();i+=2){
			string axis = tokens[i];
			double angle = strtod(tokens[i+1].c_str(),NULL);
			pair<string,double> one_rotation(axis,angle);
			rotations.push_back(one_rotation);
		}
	}*/
	vector< pair<string,double> > rotations;

	rotations.push_back(make_pair("x", atof(argv[2])));
	rotations.push_back(make_pair("y", atof(argv[2])));
	rotations.push_back(make_pair("z", atof(argv[2])));
	return rotations;
}

Matrix3d rot_x(Matrix3d matrix, double angle){
	Matrix3d trans_mat;
	trans_mat << 	1,     0     ,      0     ,
					0, cos(angle), -sin(angle),
					0, sin(angle),  cos(angle);
	return trans_mat*matrix;
}

Matrix3d rot_y(Matrix3d matrix, double angle){
	Matrix3d trans_mat;
	trans_mat << 	cos(angle),     0     , sin(angle),
					    0     ,     1     ,     0     ,
				   -sin(angle),     0     , cos(angle);
	return trans_mat*matrix;
}

Matrix3d rot_z(Matrix3d matrix, double angle){
	Matrix3d trans_mat;
	trans_mat << 	cos(angle), -sin(angle),    0    ,
					sin(angle),  cos(angle),    0    ,
					    0     ,      0     ,    1    ;
	return trans_mat*matrix;
}

Matrix3d rotate(Matrix3d matrix, double angle, string axis){
	if(axis == "x" || axis == "X")
		matrix = rot_x(matrix,angle);
	else if(axis == "y" || axis == "Y")
		matrix = rot_y(matrix, angle);
	else if(axis == "z" || axis == "Z"){
		matrix = rot_z(matrix, angle);
	}
	return matrix;
}

Matrix3d get_transfortation_matrix(vector< pair<string,double> >& rotations, double scale){
	Matrix3d trans_mat = Matrix3d::Identity(3,3);
	for(int i=0;i<rotations.size();i++){
		string axis = rotations[i].first;
		double angle = (rotations[i].second*PI)/180;
		trans_mat = rotate(trans_mat,angle,axis);
	}
	return trans_mat*scale;
}

void move_obj_to_center(vector<Vector3d>& vertices, Vector3d obj_cent){
	for(int i=0;i<vertices.size();i++){
		vertices[i] -= obj_cent;
	}
}

Vector3d get_object_center(vector<Vector3d>& vertices){
	Vector3d center(0,0,0);
	for(int i=0;i<vertices.size();i++){
		center = center + vertices[i];
	}
	if(vertices.size()>0){
		center = center/(vertices.size());
	}
	return center;
}

vector<Vector3d> get_transformed_vertices(vector<Vector3d> vertices,
	vector< pair<string,double> >& rotations, double scale,
	pair<string,double>& projection){
	vector<Vector3d> transformed_vertices;
	Matrix3d tranformation_matrix = get_transfortation_matrix(rotations,scale);
	for(int i=0;i<vertices.size();i++){
		transformed_vertices.push_back(tranformation_matrix*vertices[i]);
	}

	Vector3d obj_cent = get_object_center(transformed_vertices);
	move_obj_to_center(transformed_vertices, obj_cent);

	return transformed_vertices;
}

string get_fill_string(Vector3i fill){
	string fill_col = "";
	fill_col += ("rgb("+to_string(fill(0))+","
					   +to_string(fill(1))+","
					   +to_string(fill(2))+")");
	return fill_col;
}

vector<Vector3d> three_diff_vertices(vector<int>& face, vector<Vector3d>& points){
	Vector3d v1 = points[face[0]-1],v2,v3;
	vector<Vector3d> vertices;
	int index = 0;
	while(index<face.size()){
		v2 = points[face[index]-1];
		if(v1==v2){
			index++;
		}
		else
			break;
	}
	if(v1 == v2){
		return vertices;
	}
	index++;
	while(index<face.size()){
		v3 = points[face[index]-1];
		if((v1==v3) || (v2==v3)){
			index++;
		}
		else
			break;
	}
	if((v1==v3) || (v2==v3)){
		return vertices;
	}
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	return vertices;
}

Vector3d get_normal(vector<int>& face, vector<Vector3d>& points){
	Vector3d cross(0,0,0);
	vector<Vector3d> vertices = three_diff_vertices(face, points);
	if(vertices.size() == 0)
		return cross;
	Vector3d a = vertices[1] - vertices[0];
	Vector3d b = vertices[2] - vertices[0];
	cross = a.cross(b);
	return cross;
}

double get_angle(Vector3d vector1, Vector3d vector2){
	return acos(vector1.dot(vector2));
}

void get_cent_z(double& sum,vector<int>& face, vector<Vector3d>& points){
	sum = 0;
	for(int i=0;i<face.size();i++){
		sum += points[face[i] - 1](2);
	}
	sum = sum/face.size();
}

Vector3d get_ele_wise_prod(Vector3d a, Vector3d b){
	Vector3d c;
	for(int i=0;i<3;i++){
		c(i) = a(i)*b(i);
	}
	return c;
}

Vector3i get_floor(Vector3d a){
	Vector3i b;
	for(int i=0;i<3;i++){
		b(i) = (int)a(i);
	}
	return b;
}

Vector3d get_half_vector(Vector3d a, Vector3d b){
	Vector3d c = a + b;
	c.normalize();
	return c;
}

Vector3d check_color(Vector3d color){
	for(int i=0;i<3;i++){
		if(color(i)<0)
			color(i)=0;
		else if(color(i)>255)
			color(i)=255;
	}
	return color;
}

vector< pair<double,int> > get_z_list(vector< vector<int> >& faces, vector<Vector3d>& vertices){
	vector< pair<double,int> > z_list;
	for(int j=0;j<faces.size();j++){
		vector<int> face = faces[j];
		double sum = 0;
		for(int i=0;i<face.size();i++){
			sum = sum + vertices[face[i]-1](2);
		}
		sum = sum/face.size();
		z_list.push_back(make_pair(sum,j));
	}
	return z_list;
}

Vector3i get_face_color(Light& light, Material& material, Vector3d& normal){
	int illum_no = material.get_illum_no();
	Vector3d color(255,255,255);
	if(illum_no==0){
		Vector3d diffuse = normal.dot(light.get_position())*
			get_ele_wise_prod(light.get_diffuse(),material.get_diffuse());
		color = check_color(get_ele_wise_prod(color, diffuse));
	}
	else if(illum_no == 1){
		Vector3d ambient = get_ele_wise_prod(light.get_ambient(),material.get_ambient());
		Vector3d ambient_color = check_color(get_ele_wise_prod(color, ambient));

		Vector3d diffuse = normal.dot(light.get_position())*
			get_ele_wise_prod(light.get_diffuse(),material.get_diffuse());
		Vector3d diffuse_color = check_color(get_ele_wise_prod(color, diffuse));

		color = check_color(ambient+diffuse);
	}
	else if(illum_no == 2){
		Vector3d ambient = get_ele_wise_prod(light.get_ambient(),material.get_ambient());
		Vector3d ambient_color = check_color(get_ele_wise_prod(color, ambient));

		Vector3d diffuse = normal.dot(light.get_position())*
			get_ele_wise_prod(light.get_diffuse(),material.get_diffuse());
		Vector3d diffuse_color = check_color(get_ele_wise_prod(color, diffuse));

		Vector3d half = get_half_vector(light.get_position(), normal);
		Vector3d specular = pow((normal.dot(half)),material.get_shininess())*
			get_ele_wise_prod(light.get_specular(),material.get_specular());
		Vector3d specular_color = check_color(get_ele_wise_prod(color, specular));
		color = check_color(ambient_color+diffuse_color+specular_color);
	}

	return get_floor(color);
}

void write_SVG_dot(ofstream& file, Vector3d vertex, Vector3i fill){
	double delta_x = (double) (IMG_WIDTH/2);
	double delta_y = (double)(IMG_HEIGHT/2);

	double cx = delta_x+vertex[0], cy = delta_y+vertex[1];
	string fill_col = get_fill_string(fill);
	file<<"<circle cx=\""<<cx<<"\" cy=\""<<cy<<"\" r=\"2\""
		<<" style=\"stroke:rgb(0,0,0);stroke-width:1;fill:"<<fill_col
		<<";stroke-opacity:1;fill-opacity:1\"/>"<<endl;
}

void write_SVG_header(ofstream& file, string title) {
  file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl
       << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\"" << endl
       << " \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << endl
       << "<svg width=\"" << IMG_WIDTH
       << "\" height=\"" << IMG_HEIGHT <<"\"" << endl
       << "xmlns=\"http://www.w3.org/2000/svg\" "
       << "xmlns:xlink= \"http://www.w3.org/1999/xlink\">" <<endl
       << "<title>"<<title<<"</title>" << endl;
}

void write_SVG_footer(ofstream& file){
	file << "</svg>" << endl;
}

void remove_duplicates(vector< vector<int> >& list){
	vector< vector<int> > new_list;
	map<vector<int>,bool> entries;
	for(int i=0;i<list.size();i++){
		vector<int> edge = list[i];
		if(entries.find(edge)==entries.end()){
			entries[edge] = true;
			new_list.push_back(edge);
		}
	}
	list = new_list;
}

vector< vector<int> > make_edge_list(vector< vector<int> >& face_list){
	vector< vector<int> > edge_list;
	for(int i=0;i<face_list.size();i++){
		int edges = face_list[i].size();
		for(int j=0;j<edges;j++){
			vector<int> new_edge;
			int vertex1 = face_list[i][j];
			int vertex2 = face_list[i][(j+1)%edges];
			if(vertex1<vertex2){
				new_edge.push_back(vertex1);
				new_edge.push_back(vertex2);
			}
			else{
				new_edge.push_back(vertex2);
				new_edge.push_back(vertex1);
			}
			edge_list.push_back(new_edge);
		}
	}
	remove_duplicates(edge_list);
	return edge_list;
}

void write_SVG_line(ofstream& file, Vector3d p1, Vector3d p2, double stroke_opacity){
	double delta_x = (double) (IMG_WIDTH/2);
	double delta_y = (double)(IMG_HEIGHT/2);
	double x1 = delta_x + p1(0), y1 = delta_y + p1(1);
	double x2 = delta_x + p2(0), y2 = delta_y + p2(1);

	file<<"<line x1=\""<<x1<<"\" y1=\""<<y1<<"\" x2=\""<<x2<<"\" y2=\""<<y2
		<<"\" style=\"stroke:rgb(0,0,0);stroke-width:2;stroke-opacity:"
		<<to_string(stroke_opacity)<<";"<<"stroke-linecap:round;\" />"<<endl;
}

void write_edges(ofstream& file,vector< vector<int> >& edge_list,
	vector<Vector3d>& points, double stroke_opacity){
	for(int i=0;i<edge_list.size();i++){
		vector<int> edge = edge_list[i];
		Vector3d point_1 = points[edge[0]-1];
		Vector3d point_2 = points[edge[1]-1];
		write_SVG_line(file, point_1, point_2, stroke_opacity);
	}
}

Vector3i get_darkened_color(Vector3i fill_col, double factor){
	fill_col(0) = (int)(((double)fill_col(0))*factor);
	fill_col(1) = (int)(((double)fill_col(1))*factor);
	fill_col(2) = (int)(((double)fill_col(2))*factor);
	return fill_col;
}

void set_image_dimension(vector<Vector3d>& vertices){
	if(vertices.size()>0){
		double max_x = vertices[0](0), max_y = vertices[0](1);
		double min_x = vertices[0](0), min_y = vertices[0](1);
		for(int i=0;i<vertices.size();i++){
			Vector3d vertex = vertices[i];
			if(max_x<abs(vertex(0)))
				max_x = abs(vertex(0));
			if(max_y<abs(vertex(1)))
				max_y = abs(vertex(1));
		}

		IMG_WIDTH = 2*max_x + 100*(1-pow(2,max_x/(-100)));
		IMG_HEIGHT = 2*max_y + 100*(1-pow(2,max_y/(-100)));
	}
}

void write_SVG_poly(ofstream& file, vector<int>& face,
	vector<Vector3d>& points, Vector3i fill,double fill_opacity,
	double stroke_opacity){
	double delta_x = (double) (IMG_WIDTH/2);
	double delta_y = (double)(IMG_HEIGHT/2);
	string str = "<path d=\"";
	for(int i=0;i<face.size();i++){
		if(i==0)
			str += "M ";
		else
			str += "L ";
		double x = delta_x + points[face[i]-1](0), y = delta_y - points[face[i]-1](1);
		str += to_string(x)+" "+to_string(y)+" ";
	}
	string fill_col = get_fill_string(fill);
	str +="Z\"";
	str += " style=\"stroke:rgb(0,0,0);stroke-width:1;stroke-linejoin:round;";
	str	+="stroke-opacity:"+ to_string(stroke_opacity)+";fill:";
	//str += " style=\"stroke:None;fill:";
	str += (fill_col);
	str +=";fill-opacity:"+to_string(fill_opacity)+"\" />\n";
	file.write(str.c_str(),str.length());
}

void write_faces(ofstream& file, vector< pair<double,int> >& z_list,
	vector< vector<int> >& face_list, vector<Vector3d>& points,
	map<vector<int>,string>& material_name_of_faces, map<string,Material>& materials,
	Light& light, bool back_faces, double stroke_opacity){

	for(int i=0;i<z_list.size();i++){
		//cout<<"Face: "<<i<<endl;
		pair<double,int> face_data = z_list[i];
		int face_no = face_data.second;
		vector<int> face = face_list[face_no];

		Material face_material = materials[material_name_of_faces[face]];
		Vector3d face_norm = get_normal(face,points);

		if(face_norm == Vector3d(0,0,0)){
			continue;
		}
		if(back_faces || face_norm(2)>0){
			face_norm.normalize();

			Vector3i fill = get_face_color(light, face_material, face_norm);
			write_SVG_poly(file, face, points, fill, face_material.get_opacity(), stroke_opacity);
		}
	}
}

vector< vector<int> > clip_faces(vector< vector<int> > faces,vector<Vector3d>& vertices,
	double observer, map<vector<int>,string>& transformed_material_of_faces,
	map<vector<int>,string> material_of_faces) {
	double screen = observer - 400;
	vector< vector<int> > transformed_faces;

	for(int j=0;j<faces.size();j++){
		vector<int> face = faces[j];
		string material_name = material_of_faces[face];
		vector<int> clipped_face;
		for(int i=0;i<face.size();i++){
			Vector3d v1 = vertices[face[i]-1];
			Vector3d v2 = vertices[face[(i+1)%face.size()]-1];
			if((v1(2)>screen) && (v2(2)<=screen)){
				double factor = (screen-v1(2))/(v2(2)-v1(2));
				double x = v1(0)+(factor*(v2(0)-v1(0)));
				double y = v1(1)+(factor*(v2(1)-v1(1)));
				double z = screen;
				Vector3d v3(x,y,z);
				vertices.push_back(v3);
				clipped_face.push_back(vertices.size());
			}
			else if((v1(2)<=screen) && (v2(2)>screen)){
				double factor = (screen-v1(2))/(v2(2)-v1(2));
				double x = v1(0)+(factor*(v2(0)-v1(0)));
				double y = v1(1)+(factor*(v2(1)-v1(1)));
				double z = screen;
				Vector3d v3(x,y,z);
				vertices.push_back(v3);
				clipped_face.push_back(face[i]);
				clipped_face.push_back(vertices.size());
			}
			else if((v1(2)<=screen) && (v2(2)<=screen)){
				clipped_face.push_back(face[i]);
			}
		}
		if(clipped_face.size()>0){
			transformed_material_of_faces[clipped_face] = material_name;
			transformed_faces.push_back(clipped_face);
		}
	}
	return transformed_faces;
}

int main(int argc, char* argv[]){
	Object_3D obj;
	string filename = "";
	string current_dir = "";


	if (argc == 5){
		current_dir = get_current_directory(argv[1]);
		if(!parse_object(argv[1], obj, current_dir))
			return 0;
		obj.setType("--face"); //Processing only face type objs
		filename = get_filename(argv[1]);
	}
	else {
		cout<<"usage: "<< argv[0] <<" <filename> xdeg ydeg zdeg\n";
		return 0;
	}

	Vector3i fill_col (255,0,0);
	Vector3d lighting (0,0,2);
	lighting.normalize();
	Light light;
	light.set_position(lighting);

	vector< pair<string,double> > rotations = get_rotations(argv);
	double scale = 100;
	pair<string,double> projection = make_pair(PARALLEL,0); //set to parallel

  cout<<"Transforming vertices..."<<endl;
	vector<Vector3d> transformed_vertices =
  		get_transformed_vertices(obj.getVertices(),rotations,scale,projection);
	cout<<"Vertices transformed."<<endl;

	vector< vector<int> > transformed_faces = obj.getFaces();
	map<vector<int>,string>transformed_material_of_faces;
	transformed_material_of_faces = obj.getMaterialOfFaces();


	bool back_faces = false; //set to false;
	double stroke_opacity = 1.0;
	set_image_dimension(transformed_vertices);

	string filename_svg = filename + ".svg";
	ofstream file;
	file.open(filename_svg.c_str());

	write_SVG_header(file,filename);

	if(obj.getType() == "face"){
		vector< vector<int> > face_list = transformed_faces;
		map<string,Material> materials = obj.getMaterials();
		vector< pair<double,int> >z_list;
		cout<<"Making face list..."<<endl;

		z_list = get_z_list(face_list, transformed_vertices);
		cout<<"Face list completed. "<<face_list.size()<<" faces are present,"<<endl;
		cout<<"And "<<transformed_vertices.size()<<" vertices are present."<<endl;

		cout<<"Sorting faces..."<<endl;
		sort(z_list.begin(),z_list.end());
		cout<<"Faces sorted..."<<endl;
		cout<< "Generating SVG file..."<<endl;
		write_faces(file,z_list,face_list,transformed_vertices,
			transformed_material_of_faces, materials, light,
			back_faces, stroke_opacity);
	}
	else{
		try{
			throw "Face Data Not Found. Ensure file contains face data.";
		}
		catch(char const* e){
			cout<<"An error occurred: "<<e<<endl;
		}
	}

	write_SVG_footer(file);
	file.close();
	cout<<"SVG file generated."<<endl;
	return 0;
}
