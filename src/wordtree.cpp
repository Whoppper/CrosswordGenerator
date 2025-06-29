#include "wordtree.hpp"
    
WordTree::WordTree()
{
    root = new TreeNode();
}

WordTree::~WordTree()
{
    delete root; 
}


void WordTree::insert(const QString& word)
{
    TreeNode* current = root;
    for (QChar ch : word)
    {
        if (!current->children.contains(ch))
        {
            current->children.insert(ch, new TreeNode());
        }
        current = current->children.value(ch);
    }
    current->isEndOfWord = true;
}


bool WordTree::search(const QString& word) const
{
    TreeNode* current = root;
    for (QChar ch : word)
    {
        if (!current->children.contains(ch))
        {
            return false;
        }
        current = current->children.value(ch);
    }
    return current != nullptr && current->isEndOfWord;
}


bool WordTree::startsWith(const QString& prefix) const
{
    TreeNode* current = root;
    for (QChar ch : prefix)
    {
        if (!current->children.contains(ch))
        {
            return false;
        }
        current = current->children.value(ch);
    }
    return true;
}


TreeNode* WordTree::findNode(const QString& prefix) const
{
    TreeNode* current = root;
    for (QChar ch : prefix)
    {
        if (!current->children.contains(ch))
        {
            return nullptr;
        }
        current = current->children.value(ch);
    }
    return current;
}


void WordTree::getAllWords(TreeNode* startNode, const QString &currentPrefix, QVector<QString> &words) const
{
    if (startNode == nullptr)
    {
        startNode = root;
    }

    if (startNode->isEndOfWord)
    {
        words.append(currentPrefix);
    }

    for (auto it = startNode->children.constBegin(); it != startNode->children.constEnd(); ++it)
    {
        QChar ch = it.key();
        TreeNode* childNode = it.value();
        getAllWords(childNode, currentPrefix + ch, words);
    }

}


void WordTree::findWordsByPattern(const QString& pattern, QVector<QString> &results) const
{
    QString currentWord;
    findWordsByPatternRecursive(root, pattern, 0, currentWord, results);
}

bool WordTree::findAnyWordByPattern(const QString& pattern) const
{
    return findAnyWordByPatternRecursive(root, pattern, 0);
}



void WordTree::findWordsByPatternRecursive(TreeNode* node, const QString& pattern, int index,
                                QString& currentWord, QVector<QString>& results) const
{
    if (node == nullptr)
    {
        return;
    }

    if (index == pattern.length())
    {
        if (node->isEndOfWord)
        {
            results.append(currentWord);
        }
        return;
    }

    QChar patternChar = pattern.at(index).toLower();

    if (patternChar == '.')
    {
        for (auto it = node->children.constBegin(); it != node->children.constEnd(); ++it)
        {
            QChar ch = it.key();
            TreeNode* childNode = it.value();
            currentWord.append(ch);
            findWordsByPatternRecursive(childNode, pattern, index + 1, currentWord, results);
            currentWord.chop(1);
        }
    } 
    else
    {
        if (node->children.contains(patternChar))
        {
            TreeNode* childNode = node->children.value(patternChar);
            currentWord.append(patternChar);
            findWordsByPatternRecursive(childNode, pattern, index + 1, currentWord, results);
            currentWord.chop(1);
        }
    }
}


bool WordTree::findAnyWordByPatternRecursive(TreeNode* node, const QString& pattern, int index) const
{
    if (node == nullptr)
    {
        return false;
    }

    if (index == pattern.length())
    {
        return node->isEndOfWord;
    }

    QChar patternChar = pattern.at(index);


    if (patternChar == '.')
    {
        for (auto it = node->children.constBegin(); it != node->children.constEnd(); ++it)
        {
            TreeNode* childNode = it.value();
            if (findAnyWordByPatternRecursive(childNode, pattern, index + 1))
            {
                return true;
            }
        }
    }
    else
    {
        if (node->children.contains(patternChar))
        {
            TreeNode* childNode = node->children.value(patternChar);
            return findAnyWordByPatternRecursive(childNode, pattern, index + 1);
        }
    }
    return false;
}


int WordTree::countWordsByPattern(const QString& pattern, int maxCount) const
{
    int count = 0;
    countWordsByPatternRecursive(root, pattern, 0, maxCount, count);
    return count;
}

int WordTree::countWordsByPatternRecursive(TreeNode* node, const QString& pattern, int index, int maxCount, int& currentCount) const
{
    if (currentCount >= maxCount || node == nullptr)
    {
        return currentCount;
    }

    if (index == pattern.length())
    {
        if (node->isEndOfWord)
        {
            currentCount++;
        }
        return currentCount;
    }

    QChar patternChar = pattern.at(index);

    if (patternChar == '.')
    {
        for (auto it = node->children.constBegin(); it != node->children.constEnd(); ++it)
        {
            TreeNode* childNode = it.value();
            countWordsByPatternRecursive(childNode, pattern, index + 1, maxCount, currentCount); 
            if (currentCount >= maxCount)
            {
                return currentCount;
            }
        }
    }
    else
    {
        if (node->children.contains(patternChar))
        {
            TreeNode* childNode = node->children.value(patternChar);
            countWordsByPatternRecursive(childNode, pattern, index + 1, maxCount, currentCount);
        }
    }
    return currentCount;
}