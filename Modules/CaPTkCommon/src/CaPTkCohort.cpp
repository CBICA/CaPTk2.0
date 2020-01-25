#include "CaPTkCohort.h"
#include "CaPTkCohortSubject.h"

captk::Cohort::Cohort(QObject* parent) :
    QObject(parent)
{

}

captk::Cohort::~Cohort()
{

}

QString captk::Cohort::GetName()
{
    return m_Name;
}

QList<captk::CohortSubject*> captk::Cohort::GetSubjects()
{
    return m_Subjects;
}

void captk::Cohort::SetName(QString name)
{
    m_Name = name;
}

void captk::Cohort::SetSubjects(QList<captk::CohortSubject*> subjects)
{
    m_Subjects = subjects;
}