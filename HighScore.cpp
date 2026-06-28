#include "HighScore.hpp"
#include <fstream>

HighScore::HighScore(std::string path) : m_path(std::move(path))
{
}

int HighScore::load()
{
    std::ifstream f(m_path);
    int v = 0;
    if (f.good()) f >> v;
    m_value = v;
    return m_value;
}

void HighScore::saveIfHigher(int score)
{
    if (score <= m_value) return;
    m_value = score;
    std::ofstream f(m_path, std::ios::trunc);
    if (f.good()) f << m_value;
}

int HighScore::value() const
{
    return m_value;
}

