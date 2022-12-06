#ifndef TOOLS_H
#define TOOLS_H

#include <string_view>
#include <filesystem>
#include <vector>


void processFile(std::filesystem::path folder, std::istream& listing);
void obscureDataSet(std::ostream& output, std::filesystem::path dataset, std::filesystem::path from_folder, std::filesystem::path to_folder,
    const std::vector<std::string>& method_descriptor);
void genTripletDataset(std::ostream& output, std::filesystem::path folder, int n);
void genTripletDataset(std::ostream& output, std::filesystem::path folder, std::filesystem::path pairFile);
void genPairDataset(std::ostream& output, std::filesystem::path folder, int n);
void genPairDataset(std::ostream& output, std::filesystem::path folder, std::filesystem::path pairFile, int n);

#endif
