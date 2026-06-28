#pragma once
#include <string>

class HighScore
{
public:
    explicit HighScore(std::string path);

    int load();
    void saveIfHigher(int score);
    int value() const;

private:
    std::string m_path;
    int m_value{};
};

