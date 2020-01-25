#include "CaPTkCohortSubject.h"

#include "CaPTkCohortStudy.h"

captk::CohortSubject::CohortSubject(QObject* parent) :
    QObject(parent)
{

}

captk::CohortSubject::~CohortSubject()
{

}

QString captk::CohortSubject::GetName()
{
    return m_Name;
}

QList<captk::CohortStudy*> captk::CohortSubject::GetStudies()
{
    return m_Studies;
}

void captk::CohortSubject::SetName(QString name)
{
    m_Name = name;
}

void captk::CohortSubject::SetStudies(QList<captk::CohortStudy*> studies)
{
    m_Studies = studies;
}