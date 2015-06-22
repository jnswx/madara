
#ifndef _MADARA_LOGGER_LOGGER_H_
#define _MADARA_LOGGER_LOGGER_H_

#include "madara/MADARA_export.h"
#include "madara/Lock_Type.h"
#include <ace/Guard_T.h>
#include <vector>
#include <string>
#include <stdio.h>

namespace Madara
{
  namespace Logger
  {

    /**
    * Logging levels available for MADARA library
    **/
    enum Log_Levels
    {
      LOG_EMERGENCY = 0,
      LOG_ALWAYS = 0,
      LOG_ERROR = 1,
      LOG_WARNING = 2,
      LOG_MAJOR = 3,
      LOG_MINOR = 4,
      LOG_TRACE = 5,
      LOG_DETAILED = 6,
      LOG_MADARA_MAX = 6
    };

    /**
     * A multi-threaded logger for logging to one or more destinations
     **/
    class MADARA_Export Logger
    {
    public:
      /**
       * Constructor
       * @param  log_to_stderr  if true, log to stderr by default
       **/
      Logger (bool log_to_stderr = true);

      /**
       * Destructor
       **/
      ~Logger ();

      /**
       * Logs a message to all available loggers
       * @param  level   the logging level @see Log_Levels
       * @param  message the message to log
       **/
      void log (int level, const char * message, ...);

      /**
       * Adds a file to the logger
       * @param  filename the file to open and add to writing
       **/
      void add_file (const std::string & filename);

      /**
       * Adds terminal to logger outputs
       **/
      void add_term (void);

      /**
       * Adds the system log
       **/
      void add_syslog (void);

      /**
       * Sets the maximum logging detail level
       * @param  level  the maximum level of log detail
       **/
      void set_level (int level);

      /**
      * Gets the tag used for syslogs
      * @return  the tag used for any system logging
      **/
      std::string get_tag (void);

      /**
      * Sets the tag used for syslogs (e.g. Android Log)
      * @param  tag  the tag to use for any system logging
      **/
      void set_tag (const std::string & tag);

      /**
      * Gets the maximum logging detail level
      * @return the maximum level of log detail
      **/
      int get_level (void);

      /**
       * Clears all log targets
       **/
      void clear (void);

    private:

      /// guard for access and changes
      typedef ACE_Guard<MADARA_LOCK_TYPE> Guard;

      /// vector of file handles
      typedef std::vector <FILE *>   File_Vectors;

      /// mutex for changes
      mutable MADARA_LOCK_TYPE mutex_;

      /// list of all log outputs
      File_Vectors files_;

      /// the maximum detail level for logging
      int level_;

      /// tracks whether terminal output has been added
      bool term_added_;

      /// tracks whether the system log has been added
      bool syslog_added_;

      /// the tag used for logging to system logs
      std::string tag_;
    };
  }
}

#include "Logger.inl"

#endif // _MADARA_LOGGER_LOGGER_H_