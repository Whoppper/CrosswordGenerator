#ifndef WORDTRESS_H
#define WORDTRESS_H

#include <QChar>
#include <QHash> 
#include <QString>
#include <QVector> 
#include <limits>

class TreeNode
{
public:

    QHash<QChar, TreeNode*> children;
    bool isEndOfWord;

    TreeNode() : isEndOfWord(false) {}

    ~TreeNode()
    {
        for (TreeNode* child : children.values())
        {
            delete child;
        }
    }
};

class WordTree {
public:
    TreeNode* root;
    TreeNode* rootReversed;

    WordTree();
    ~WordTree();

    void insert(const QString& word);
    bool search(const QString& word) const;
    bool startsWith(const QString& prefix) const;
    bool endsWith(const QString& suffix) const;

    TreeNode* findNode(const QString& prefix) const;

    void getAllWords(TreeNode* startNode, const QString &currentPrefix, QVector<QString> &words) const;

    void findWordsByPattern(const QString& pattern, QVector<QString> &results) const;
    bool findAnyWordByPattern(const QString& pattern) const;
    int countWordsByPattern(const QString& pattern, int maxCount = std::numeric_limits<int>::max()) const;

private:

    bool chooseNormalTree(const QString& pattern)const ;

    void findWordsByPatternRecursive(TreeNode* node, const QString& pattern, int index,
                                   QString& currentWord, QVector<QString>& results) const;
    bool findAnyWordByPatternRecursive(TreeNode* node, const QString& pattern, int index) const;
    int countWordsByPatternRecursive(TreeNode* node, const QString& pattern, int index, int maxCount, int& currentCount) const;


    void findWordsByPatternRecursiveReversed(TreeNode* node, const QString& pattern, int index,
                                            QString& currentWord, QVector<QString>& results,
                                            const QString& originalPattern) const;
    bool findAnyWordByPatternRecursiveReversed(TreeNode* node, const QString& pattern, int index) const;
    int countWordsByPatternRecursiveReversed(TreeNode* node, const QString& pattern, int index, int maxCount, int& currentCount) const;
};

#endif 