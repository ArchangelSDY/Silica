#include "DoNothingFilter.h"

DoNothingFilter::DoNothingFilter(AbstractPlayListFilter *parentFilter) :
    AbstractPlayListFilter(parentFilter)
{
}

bool DoNothingFilter::filter(const ImagePtr &)
{
    return true;
}
