// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2XI_SOLVER_H
#define MCRL2XI_SOLVER_H

#include <QObject>
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/classic_enumerator.h"

class Solver : public QObject
{
    Q_OBJECT
  public:
    Solver();
    static const std::string className;

  signals:
    void solvedPart(QString output);
    void solved();
    
  public slots:
    void setRewriter(QString rewriter);
    void solve(QString specification, QString dataExpression);
    void abort();
    
  private:

    mcrl2::data::rewrite_strategy m_rewrite_strategy;

    mcrl2::data::data_specification m_data_spec;
    atermpp::set <mcrl2::data::variable > m_vars;

    bool m_abort;
    bool m_parsed;
    QString m_specification;
};

#endif // MCRL2XI_SOLVER_H
