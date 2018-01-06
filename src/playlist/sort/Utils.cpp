#include <QRegularExpression>

bool ImageNameLessThan(const QString &left, const QString &right)
{
    static QRegularExpression numberRegex("(\\d+)");

    if (left.split(numberRegex) == right.split(numberRegex)) {
        // Names are similar except number parts

        // Extract number parts from left
        QList<int> leftNumbers;
        QRegularExpressionMatchIterator leftIter =
            numberRegex.globalMatch(left);
        while (leftIter.hasNext()) {
            QRegularExpressionMatch m = leftIter.next();
            leftNumbers << m.captured(1).toInt();
        }

        // Extract number parts from right
        QList<int> rightNumbers;
        QRegularExpressionMatchIterator rightIter =
            numberRegex.globalMatch(right);
        while (rightIter.hasNext()) {
            QRegularExpressionMatch m = rightIter.next();
            rightNumbers << m.captured(1).toInt();
        }

        int len = qMin(leftNumbers.length(), rightNumbers.length());
        if (len > 0) {
            // Compare using first different number
            for (int i = 0; i < len; ++i) {
                int delta = rightNumbers[i] - leftNumbers[i];
                if (delta != 0) {
                    return delta > 0;
                }
            }
        }
    }

    // Fallback to string comparison
    return left < right;
}
