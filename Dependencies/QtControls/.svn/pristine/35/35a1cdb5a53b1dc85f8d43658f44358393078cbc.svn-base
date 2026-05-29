#include "QuickFilterItem.h"
#include "boolinq.h"

QList<QuickFilterCategory*> QuickFilterRoot::AllChildCategoriesRecursive()
{
    struct IntStr {
        QString Name;
        int Priority;
        bool operator<(const IntStr& right)
        {
            if(Priority < right.Priority)
                return true;
            else if(Priority > right.Priority)
                return false;
            return Name < right.Name;
        }
    };
    
    return boolinq::from(m_cats).orderBy([](QuickFilterCategory* cat){return IntStr{cat->GetName(), cat->Priority()};}).toQList();
}
