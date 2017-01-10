/**
 * @file		doors.h
 * @author	daniel
 * @brief	Definiert die Zustandsmaschine und die Logik
 */

#ifndef DOORS_H_
#define DOORS_H_

#include <memory>

// Backend einbinden
#include <boost/msm/back/state_machine.hpp>
// boost::asio wird für die Events und das Timeout verwendet
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>

// Forward-Deklaration der Definition damit der Typ der FSM bekannt ist
class DoorsStateMachineDef;
typedef boost::msm::back::state_machine<DoorsStateMachineDef> DoorsStateMachine;


/**
 * Automatische Tür, die bei Bewegung öffnet und nach Timeout wieder schließt.
 */
class Doors
{
public:
   /// Konstruktor
   Doors();
   /// Destruktor
   ~Doors();

   /// Thread-Funktion, in der die Ablaufsteuerung ausgeführt wird.
   void run();
   /// Beendet Thread-Funktion
   void stop();

   // Logik
   /// Öffne die Tür.
   void open();
   /// Schließe die Tür.
   void close();

   // Events
   /// Der Bewegungsmelder schlägt aus.
   void motionDetected();
   /// Die Tür wird gesperrt/entsperrt.
   void setLocked(bool locked);
   /// Ist die Tür gesperrt?
   bool isLocked() const;

   // State-Machine-Aktionen
   /// Startet den Timer neu
   void restartTimer();

private:
   /// Das eigentliche Event-Handling wird von einem io_service erledigt.
   boost::asio::io_service mIoService;
   boost::asio::deadline_timer mTimer;

   /// Hier instanziieren wir die State-Machine.
   std::unique_ptr<DoorsStateMachine> mStateMachine;

   /// Ist die Tür gesperrt?
   bool mLocked;
};

#endif /* DOORS_H_ */
