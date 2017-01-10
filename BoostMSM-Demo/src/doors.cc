/**
 * @file		doors.cc
 * @author	daniel
 * @brief	Implementation der Zustandsmaschine
 */

#include "doors.h"

#include <iostream>

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/back/state_machine.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using msm::front::none;
using msm::front::Row;


// Events sind (leere) Klassen, können aber auch Argumente haben
struct MotionDetected {};
struct Timeout {};

/**
 * Die Definition der State-Machine enthält die Zustandstabelle und damit auch
 * die Übergänge.
 */
class DoorsStateMachineDef: public msm::front::state_machine_def<DoorsStateMachineDef>
{
public:
   /// Referenz auf die Doors-Klasse, die die eigentliche Funktionalität hat.
   Doors& doors;
   DoorsStateMachineDef(Doors& d)
   : doors(d)
   {}

   /*
    * Zustände sind von boost::msm::front::state abgeleitet. Sie können optional
    * eine on_entry und/oder on_exit Funktion haben.
    */

   /// Zustand: Tür geschlossen
   struct DoorsClosed: public msm::front::state<>
   {
      template<class Event, class FSM>
      void on_entry(const Event&, FSM& fsm)
      {
         //std::cout << "Zustand: DoorsClosed\n";
         fsm.doors.close();
      }
   };
   /// Zustand: Türen offen
   struct DoorsOpen: public msm::front::state<>
   {
      template<class Event, class FSM>
      void on_entry(const Event&, FSM& fsm)
      {
         //std::cout << "Zustand: DoorsOpen\n";
         fsm.doors.open();
      }
   };

   /// Der Initialzustand: hier fängt die State-Machine an
   typedef DoorsClosed initial_state;

   /**
    * Aktion: Starte den Timer (neu).
    */
   struct RestartTimer
   {
      template<class Event, class FSM, class SourceState, class TargetState>
      void operator()(const Event&, FSM& fsm, SourceState&, TargetState&) const
      {
         // den Aufruf an die Doors-Klasse weiterleiten
         fsm.doors.restartTimer();
      }
   };

   /**
    * Guard: Stelle sicher, dass die Tür nicht abgeschlossen ist.
    */
   struct NotLocked
   {
      template<class Event, class FSM, class SourceState, class TargetState>
      bool operator()(const Event&, FSM& fsm, SourceState&, TargetState&) const
      {
         return !fsm.doors.isLocked();
      }
   };

   /**
    * Die Zustandstabelle mit allen Zuständen, Events, Transitionen und Guards.
    * Fehlende Aktionen und Guards sind "none".
    */
   struct transition_table: mpl::vector<
   //    Start           Event              Next              Action            Guard
   // +--------------+----------------+------------+--------------+------------------+
   Row < DoorsClosed,  MotionDetected,  DoorsOpen,   RestartTimer,  NotLocked >,

   Row < DoorsOpen,    MotionDetected,  DoorsOpen,   RestartTimer >,
   Row < DoorsOpen,    Timeout,         DoorsClosed >
   > {};

   /**
    * Wird für alle Transitionen aufgerufen, die in der Transitionstabelle
    * nicht vorkommen.
    */
   template <class FSM,class Event>
   void no_transition(Event const& ,FSM&, int)
   {
      // nothing (ignored by design)
   }
};


// Konstruktor
Doors::Doors()
: mIoService(), mTimer(mIoService),
  mStateMachine(new DoorsStateMachine(boost::ref(*this))),
  mLocked(false)
{}

Doors::~Doors()
{
}

void Doors::run()
{
   boost::asio::io_service::work work(mIoService);
   // State-Machine starten und auf Events warten
   mStateMachine->start();
   mIoService.run();
}
void Doors::stop()
{
   mIoService.stop();
}

void Doors::open()
{
   std::cout << "\r[................]\n";
}

void Doors::close()
{
   std::cout << "\r........[]........\n";
}

void Doors::setLocked(bool locked)
{
   mIoService.post([this,locked]() {
      mLocked = locked;
   });
}
bool Doors::isLocked() const
{
   return mLocked;
}

void Doors::restartTimer()
{
   mTimer.expires_from_now(boost::posix_time::seconds(5));
   mTimer.async_wait([this](const boost::system::error_code& ec) {
      if (!ec) mStateMachine->process_event(Timeout());
   });
}

void Doors::motionDetected()
{
   // über den I/O-Service im richtigen Thread abarbeiten
   mIoService.post([this]() {
      mStateMachine->process_event(MotionDetected());
   });
}
