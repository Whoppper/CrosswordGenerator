#include "wordtree.hpp"

#include "logger.hpp"
    
WordTree::WordTree()
{
    root = new TreeNode();
    rootReversed = new TreeNode();
}

WordTree::~WordTree()
{
    delete root;
    delete rootReversed;
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

    QString reversedWord = word;
    std::reverse(reversedWord.begin(), reversedWord.end());

    current = rootReversed;
    for (QChar ch : reversedWord)
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

bool WordTree::endsWith(const QString& suffix) const
{
    QString reversedSuffix = suffix;
    std::reverse(reversedSuffix.begin(), reversedSuffix.end());

    TreeNode* current = rootReversed;
    for (QChar ch : reversedSuffix)
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


bool WordTree::chooseNormalTree(const QString& pattern) const
{
    // TODO peut etre mettre des poids sur les caracteres pour choisir l'arbre
    // pour les cas comme ".a....azert.."
    // mais pas sur que ça soit mieux et la flemme de tester pour le moment
    int i = 0;
    int j = pattern.length() - 1;
    return true;
    while (i <= j)
    {
        bool charFromStartIsFixed = (pattern.at(i) != '.');
        bool charFromEndIsFixed = (pattern.at(j) != '.');

        if (charFromStartIsFixed && !charFromEndIsFixed)
            return true; 
        if (!charFromStartIsFixed && charFromEndIsFixed)
        {
            return false; 
        }
            
        i++;
        j--;
    }

    return true;
}


void WordTree::findWordsByPattern(const QString& pattern, QVector<QString> &results) const
{
    QString currentWord;

    if (chooseNormalTree(pattern))
    {
        findWordsByPatternRecursive(root, pattern, 0, currentWord, results);
    }
    else
    {
        QString reversedPattern = pattern;
        std::reverse(reversedPattern.begin(), reversedPattern.end());
        findWordsByPatternRecursiveReversed(rootReversed, reversedPattern, 0, currentWord, results, pattern);
        for (QString& word : results)
        {
            std::reverse(word.begin(), word.end());
        }
    }
}

bool WordTree::findAnyWordByPattern(const QString& pattern) const
{
    if (chooseNormalTree(pattern))
    {
        return findAnyWordByPatternRecursive(root, pattern, 0);
    }
    else 
    {
        QString reversedPattern = pattern;
        std::reverse(reversedPattern.begin(), reversedPattern.end());
        return findAnyWordByPatternRecursiveReversed(rootReversed, reversedPattern, 0);
    }
}

int WordTree::countWordsByPattern(const QString& pattern, int maxCount) const
{
    int count = 0;
    if (chooseNormalTree(pattern))
    {
        countWordsByPatternRecursive(root, pattern, 0, maxCount, count);
    }
    else
    {
        QString reversedPattern = pattern;
        std::reverse(reversedPattern.begin(), reversedPattern.end());
        countWordsByPatternRecursiveReversed(rootReversed, reversedPattern, 0, maxCount, count);
    }
    return count;
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




void WordTree::findWordsByPatternRecursiveReversed(TreeNode* node, const QString& pattern, int index,
                                                QString& currentWord, QVector<QString>& results,
                                                const QString& originalPattern) const
{
    if (node == nullptr)
    {
        return;
    }

    if (index == pattern.length())
    {
        if (node->isEndOfWord)
        {
            // Le mot est construit à l'envers, il sera inversé après l'appel initial
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
            findWordsByPatternRecursiveReversed(childNode, pattern, index + 1, currentWord, results, originalPattern);
            currentWord.chop(1);
        }
    }
    else
    {
        if (node->children.contains(patternChar))
        {
            TreeNode* childNode = node->children.value(patternChar);
            currentWord.append(patternChar);
            findWordsByPatternRecursiveReversed(childNode, pattern, index + 1, currentWord, results, originalPattern);
            currentWord.chop(1);
        }
    }
}

bool WordTree::findAnyWordByPatternRecursiveReversed(TreeNode* node, const QString& pattern, int index) const
{
    if (node == nullptr)
    {
        return false;
    }

    if (index == pattern.length())
    {
        return node->isEndOfWord;
    }

    QChar patternChar = pattern.at(index).toLower();

    if (patternChar == '.')
    {
        for (auto it = node->children.constBegin(); it != node->children.constEnd(); ++it)
        {
            TreeNode* childNode = it.value();
            if (findAnyWordByPatternRecursiveReversed(childNode, pattern, index + 1))
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
            return findAnyWordByPatternRecursiveReversed(childNode, pattern, index + 1);
        }
    }
    return false;
}

int WordTree::countWordsByPatternRecursiveReversed(TreeNode* node, const QString& pattern, int index, int maxCount, int& currentCount) const
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

    QChar patternChar = pattern.at(index).toLower();

    if (patternChar == '.')
    {
        for (auto it = node->children.constBegin(); it != node->children.constEnd(); ++it)
        {
            TreeNode* childNode = it.value();
            countWordsByPatternRecursiveReversed(childNode, pattern, index + 1, maxCount, currentCount);
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
            countWordsByPatternRecursiveReversed(childNode, pattern, index + 1, maxCount, currentCount);
        }
    }
    return currentCount;
}