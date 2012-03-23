<?php
// $Id: phrase_captcha.inc,v 1.1 2008/02/13 21:51:51 soxofaan Exp $

/**
 * function for generating a random nonsense word of a given number of characters
 */
function _phrase_captcha_generate_nonsense_word($characters) {
  $vowels = 'bcdfghjklmnpqrstvwxyz';
  $consonants = 'aeiou';
  $vowel_max = strlen($vowels) - 1;
  $consonant_max = strlen($consonants) - 1;
  $word = '';
  $o = mt_rand(0, 1); // randomly start with vowel or consonant
  for ($i = 0; $i < $characters ; ++$i) {
    if (($i+$o) % 2) {
      $word .= $consonants[mt_rand(0, $consonant_max)];
    }
    else {
      $word .= $vowels[mt_rand(0, $vowel_max)];
    }
  }
  return $word;
}

/**
 * function for generating an array of words
 */
function _phrase_captcha_generate_words($num) {
  $words = array();
  if (variable_get('phrase_captcha_words', PHRASE_CAPTCHA_GENERATE_NONSENSE_WORDS) == PHRASE_CAPTCHA_USER_DEFINED_WORDS) {
    // use user defined words
    $uwords = _text_captcha_word_pool_get_content('phrase_captcha_userdefined_word_pool', NULL, '', TRUE);
    switch ($num) {
      case 0: break;
      case 1:
        $words[] = $uwords[array_rand($uwords, $num)];
        break;
      default:
        $keys = array_rand($uwords, $num);
        foreach ($keys as $key) {
          $words[] = $uwords[$key];
        }
        break;
    }
  }
  else {
    // generate nonsense words
    for ($w=0; $w<$num; ++$w) {
      $words[] = _phrase_captcha_generate_nonsense_word(mt_rand(3, 7));
    }
  }
  return $words;
}

/**
 * function that returns a textual represention of an ordinal
 */
function _phrase_captcha_ordinal($n) {
  $ordinalmap = array(1 => t('first'), 2 => t('second'), 3 => t('third'),
    4 => t('fourth'), 5 => t('fifth'), 6 => t('sixth'), 7 => t('seventh'),
    8 => t('eighth'), 9 => t('ninth'), 10 => t('tenth'));
  if (array_key_exists($n, $ordinalmap)) {
    return $ordinalmap[$n];
  }
  else {
    return "{$n}th";
  }
}


function _phrase_captcha_available_word_challenges() {
  return array(
    '_phrase_captcha_word_question_word_index' => 'Word index',
    '_phrase_captcha_word_question_alphabetical_misplaced' => 'Alphabetical misplacement',
    '_phrase_captcha_word_question_double_occurence' => 'Double occurence',
  );
}

function _phrase_captcha_enabled_word_challenges() {
  $word_challenges = variable_get('phrase_captcha_word_selection_challenges', array());
  if ($word_challenges) {
    return array_keys(array_filter($word_challenges));
  }
  else {
    return array_keys(_phrase_captcha_available_word_challenges());
  }
}

function _phrase_captcha_word_question_word_index($words) {
  $key = array_rand($words, 1);
  $answer = $words[$key];
  if (mt_rand(0, 1)) {
    $description = t('What is the @nth word in the CAPTCHA phrase above?',
      array('@nth' => _phrase_captcha_ordinal($key + 1)));
  }
  else {
    $n = count($words) - $key;
    if ($n == 1) {
      $description = t('What is the last word in the CAPTCHA phrase above?');
    }
    else {
      $description = t('What is the @nth last word in the CAPTCHA phrase above?',
        array('@nth' => _phrase_captcha_ordinal($n)));
    }
  }
  return array($words, $description, $answer);
}

function _phrase_captcha_word_question_alphabetical_misplaced($words) {
  // sort the words
  mt_rand(0, 1) ? sort($words) : rsort($words);
  // pick a word and its new destination
  // new destination has to be at least 2 places from the original place,
  // otherwise it could lead to something like swapping two neighbours,
  // in which case there is no unique answer.
  $from = $to = 0;
  while (abs($from - $to) < 2) {
    $from = array_rand($words, 1);
    $to = array_rand($words, 1);
  }
  // get the word
  $answer = $words[$from];
  // move the word from $from to $to
  unset($words[$from]);
  array_splice($words, $to, 0, $answer);
  // build the description
  $description = t('Which word does not follow the alphabetical order in the CAPTCHA phrase above?');
  return array($words, $description, $answer);
}

function _phrase_captcha_word_question_double_occurence($words) {
  // assure single occurence of each word
  $words = array_unique($words);
  // pick a word
  $key = array_rand($words, 1);
  $answer = $words[$key];
  // replace another word with it
  while (($pos = array_rand($words, 1)) == $key) {
    // NOP aka NOOP aka pass
  }
  array_splice($words, $pos, 1, $answer);
  $description = t('Which word occurs two times in the CAPTCHA phrase above?');
  return array($words, $description, $answer);
}
