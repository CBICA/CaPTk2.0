#include "CaPTkCohortSubject.h"

#include "CaPTkCohortStudy.h"

captk::CohortSubject::CohortSubject()
{

}

captk::CohortSubject::~CohortSubject()
{

}

QString captk::CohortSubject::GetName()
{
    return m_Name;
}

QList<QSharedPointer<captk::CohortStudy>> captk::CohortSubject::GetStudies()
{
    return m_Studies;
}

void captk::CohortSubject::SetName(QString name)
{
    m_Name = name;
}

void captk::CohortSubject::SetStudies(QList<QSharedPointer<captk::CohortStudy>> studies)
{
    m_Studies = studies;
}