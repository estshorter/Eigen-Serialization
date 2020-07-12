#include "cereal_eigen.h"
#include <random>
#include <fstream>
#include <filesystem>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

namespace fs = std::filesystem;

class TestClass {
public:
	TestClass() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> distd(-1.0, 1.0);
		std::uniform_int_distribution<int> disti(0, 100);

		mat = Eigen::MatrixXd::NullaryExpr(5, 3, [&]() {return distd(gen); });;
		arr << disti(gen), disti(gen), disti(gen);
		vec3d << distd(gen), distd(gen), distd(gen);;
	}
	void Print() {
		std::cout << "mat: " << std::endl << mat << std::endl;
		std::cout << "arr: " << arr << std::endl;
		std::cout << "evec: " << vec3d << std::endl;
	}
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(vec3d), CEREAL_NVP(arr), CEREAL_NVP(mat));
	}

	Eigen::MatrixXd mat;
	Eigen::Vector3d vec3d;
	Eigen::Array<int, 1, 3> arr;
};

int main() {
	fs::create_directory("data");
	fs::path path_binary = "data/class.cereal";
	fs::path path_json = "data/class.json";
	auto t = TestClass();
	t.Print();
	{
		std::ofstream out(path_binary, std::ios::binary);
		cereal::BinaryOutputArchive archive_o(out);
		archive_o(CEREAL_NVP(t));

		std::ofstream out2(path_json);
		cereal::JSONOutputArchive archive_o2(out2);
		archive_o2(CEREAL_NVP(t));
	}

	std::cout << std::endl;
	std::cout << "Load from a binary file" << std::endl;

	TestClass t_binary;
	{
		std::ifstream in(path_binary, std::ios::binary);
		cereal::BinaryInputArchive archive_i(in);
		archive_i(t_binary);
	}
	t_binary.Print();

	std::cout << std::endl;
	std::cout << "Load from a json file" << std::endl;
	TestClass t_json;
	{
		std::ifstream in2(path_json);
		cereal::JSONInputArchive archive_i2(in2);
		archive_i2(t_json);
	}
	t_json.Print();
}