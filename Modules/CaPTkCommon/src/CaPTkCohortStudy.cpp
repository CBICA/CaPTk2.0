#include "CaPTkCohortStudy.h"

#include "CaPTkCohortSeries.h"

captk::CohortStudy::CohortStudy(QObject* parent) :
    QObject(parent)
{

}

captk::CohortStudy::~CohortStudy()
{

}

QString captk::CohortStudy::GetName()
{
    return m_Name;
}

QList<captk::CohortSeries*> captk::CohortStudy::GetSeries()
{
    return m_Series;
}

void captk::CohortStudy::SetName(QString name)
{
    m_Name = name;
}

void captk::CohortStudy::SetSeries(QList<captk::CohortSeries*> series)
{
    m_Series = series;
}