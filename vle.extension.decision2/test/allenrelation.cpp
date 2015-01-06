/*
 * @file test/allenrelation.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_relation_allen
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <iostream>
#include <iterator>
#include <vle/value/Double.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/vle.hpp>

struct F
{
    vle::Init app;

    F() : app()
    {
    }

    ~F()
    {
    }
};

BOOST_GLOBAL_FIXTURE(F)

namespace vmd = vle::extension::decision;

namespace vle { namespace extension { namespace decision { namespace ex {

    class Before: public vmd::KnowledgeBase
    {
    public:
        Before()
        {
            addActivity("A", 0.0, 2.0);
            addActivity("B", 3.0, 5.0);

            // B needs to start at time 3.0.
            addFinishToStartConstraint("A", "B", 0.0, 2.0);
        }

        virtual ~Before() {}
    };

    class Meets : public vmd::KnowledgeBase
    {
    public:
        Meets()
        {
            addActivity("A");
            addActivity("B");

            // B needs to start when A finishes.
            addFinishToStartConstraint("A", "B", 0.0, 0.0);
        }

        virtual ~Meets() {}
    };

    class Overlaps: public vmd::KnowledgeBase
    {
    public:
        Overlaps()
        {
            addActivity("A");
            addActivity("B");

            // B needs to start at begin time of A + 1.0.
            addStartToStartConstraint("A", "B", 1.0, 2.0);
        }

        virtual ~Overlaps() {}
    };

    class During: public vmd::KnowledgeBase
    {
    public:
        During()
        {
            addActivity("A");
            addActivity("B");

            // B needs to start at begin time of A + 1.0.
            // A needs to finished at end time of B + 1.0.
            addStartToStartConstraint("A", "B", 1.0, 1.0);
            addFinishToFinishConstraint("B", "A", 1.0);
        }

        virtual ~During() {}
    };

    class Starts: public vmd::KnowledgeBase
    {
    public:
        Starts()
        {
            addActivity("A");
            addActivity("B");

            // A and B need to start at the same time.
            addStartToStartConstraint("A", "B", 0.0, 0.0);
        }

        virtual ~Starts() {}
    };

    class StartsFailed: public vmd::KnowledgeBase
    {
    public:
        StartsFailed()
        {
            addActivity("A", 1.0, 2.0);
            addActivity("B");

            // A and B need to start at the same time.
            addStartToStartConstraint("A", "B", 0.0, 0.0);
        }

        virtual ~StartsFailed() {}
    };


    class Finishes: public vmd::KnowledgeBase
    {
    public:
        Finishes()
        {
            addActivity("A");
            addActivity("B");

            // A and B need to finish at the same time.
            addFinishToFinishConstraint("A", "B", 0.0);
        }

        virtual ~Finishes() {}
    };

    class Equal: public vmd::KnowledgeBase
    {
    public:
        Equal()
        {
            addActivity("A");
            addActivity("B");

            // A and B need to start and finish a the same time.
            addStartToStartConstraint("A", "B", 0.0, 0.0);
            addFinishToFinishConstraint("A", "B", 0.0, 0.0);
        }

        virtual ~Equal() {}
    };

}}}} // namespace vle ext decision ex

BOOST_AUTO_TEST_CASE(Before1)
{
    vmd::ex::Before base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(2.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(3.0);
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(4.0);
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}

BOOST_AUTO_TEST_CASE(Before2)
{
    vmd::ex::Before base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("A", 1.0);

    base.processChanges(2.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(3.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(4.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(5.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(5.1);
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(6.0);
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
}

BOOST_AUTO_TEST_CASE(Meets1)
{
    vmd::ex::Meets base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("A", 1.0);

    base.processChanges(2.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(3.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("B", 3.0);

    base.processChanges(4.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}

BOOST_AUTO_TEST_CASE(Overlaps1)
{
    vmd::ex::Overlaps base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(3.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 0.0);
    base.setActivityDone("B", 0.0);

    base.processChanges(4.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}

BOOST_AUTO_TEST_CASE(Overlaps2)
{
    vmd::ex::Overlaps base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(3.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 3.0);

    base.processChanges(4.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("B", 4.0);

    base.processChanges(5.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}

BOOST_AUTO_TEST_CASE(During1)
{
    vmd::ex::During base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(3.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 3.0);

    base.processChanges(4.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(0));
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(5.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(0));
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
}

BOOST_AUTO_TEST_CASE(During2)
{
    vmd::ex::During base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(3.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("B", 3.0);

    base.processChanges(3.5);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("A", 3.9);

    base.processChanges(4.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}

BOOST_AUTO_TEST_CASE(Starts1)
{
    vmd::ex::Starts base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("B", 0.0);

    base.processChanges(2.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("A", 0.0);

    base.processChanges(3.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}

BOOST_AUTO_TEST_CASE(Starts2)
{
    vmd::ex::StartsFailed base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}

BOOST_AUTO_TEST_CASE(finishes)
{
    vmd::ex::Finishes base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    base.setActivityDone("A", 2.0);

    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(3.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(4.0);
    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
}

BOOST_AUTO_TEST_CASE(Equal)
{
    vmd::ex::Equal base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 2.0);
    base.setActivityDone("B", 2.0);
    base.processChanges(2.0);

    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(0));
}

BOOST_AUTO_TEST_CASE(EqualFail)
{
    vmd::ex::Equal base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 2.0);
    base.processChanges(2.0);

    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(2.1);

    lst = base.endedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
}
