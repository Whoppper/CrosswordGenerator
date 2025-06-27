#include <QChar>
#include <QHash> 
#include <QString>
#include <QVector> 


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

    WordTree();
    ~WordTree();

    void insert(const QString& word);
    bool search(const QString& word) const;
    bool startsWith(const QString& prefix) const;
    TreeNode* findNode(const QString& prefix) const;
    void getAllWords(TreeNode* startNode, const QString &currentPrefix, QVector<QString> &words) const;
    void findWordsByPattern(const QString& pattern, QVector<QString> &results) const;
    bool findAnyWordByPattern(const QString& pattern) const;

private:
    void findWordsByPatternRecursive(TreeNode* node, const QString& pattern, int index,
                                   QString& currentWord, QVector<QString>& results) const;
    bool findAnyWordByPatternRecursive(TreeNode* node, const QString& pattern, int index) const;
};

