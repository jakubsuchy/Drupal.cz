// $Id: advpoll-vote.js,v 1.1.2.11.2.3 2009/01/31 19:27:49 chriskennedy Exp $

/*
* Submit advpoll forms with ajax
*/
Drupal.behaviors.attachVoteAjax = function(context) {
  $("form.advpoll-vote", context).each(function() {
    var thisForm = this;
    var options = {
      dataType: "json",
      after: function(data) {
        // Remove previous messages
        $("div.messages").remove(); 
        
        // Insert response
        if (data.errors) {
          $(data.errors).insertBefore(thisForm).fadeIn();
        }
        else {
          $(thisForm).hide(); 
          $(data.statusMsgs).insertBefore(thisForm).fadeIn();
          $(data.response).insertBefore(thisForm);
        }

        // Re-enable the Vote button, in case there was an error message.
        $(".form-submit", thisForm).removeAttr("disabled");

      },
      before: function() {
        // Disable the Vote button.
        $(".form-submit", thisForm).attr("disabled", "disabled");
      }
    };
    // Tell the server we are passing the form values with ajax and attach the function
    $("input.ajax", thisForm).val(true);
    $(this).ajaxForm(options);
  });
};

Drupal.behaviors.handleWriteins = function(context) {
  $("form.advpoll-vote:not(.handleWriteins-processed)", context).addClass("handleWriteins-processed").each(function() {
    var poll = this;
    if ($(".writein-choice", poll).length == 0) {
      // No write-ins in this poll.
      return;
    }
    var ranOnce = false;
    // Toggle display of the write-in text box for radios/checkboxes.
    $(".vote-choices input[@type=radio], .vote-choices input[@type=checkbox]", poll).click(function() {
      var isLast = $(this).val() == $(".vote-choices input[@type=radio]:last, .vote-choices input[@type=checkbox]:last", poll).val();
      var type = $(this).attr("type"); 
      // The logic here is tricky but intentional.
      if (isLast || type == "radio") {
        var showChoice = isLast && (type == "radio" || $(this).attr("checked"));
        if (!ranOnce && showChoice) {
          // If this is our first time, clone the Drupal-added write-in element
          // and add a new one next to the checkbox, then delete the old one.
          $(".writein-choice input", poll).clone().addClass("writein-choice").insertAfter($(this).parent()).end().parent().parent().remove();
          ranOnce = true;
        }
        $(".writein-choice", poll).css("display", showChoice ? "inline" : "none");
        if (showChoice) {
          $(".writein-choice", poll)[0].focus();
        }
        else {
          $(".writein-choice", poll).val("");
        }
      }
    });
  
    // Toggle display of the write-in text box for select boxes.
    // Fire on change() rather than click(), for Safari compatibility.
    $(".vote-choices select:last", poll).change(function() {
      if (!ranOnce) {
        // If this is our first time, clone the Drupal-added write-in element
        // and add a new one next to the checkbox, then delete the old one.
        $(".writein-choice input", poll).clone().addClass("writein-choice").insertAfter($(this)).end().parent().parent().remove();
        ranOnce = true;
      }
      var showChoice = $(this).val() > 0;
      var alreadyVisible = $(".writein-choice", poll).css("display") == "inline";
      $(".writein-choice", poll).css("display", showChoice ? "inline" : "none");
      if (!showChoice) {
        $(".writein-choice", poll).val("");
      }
      else if (!alreadyVisible) {
        $(".writein-choice", poll)[0].focus();
      }
    });
  });
};

Drupal.behaviors.rankingDragAndDrop = function(context) {
  $('form.advpoll-vote.drag-and-drop:not(.advpoll-drag-and-drop-processed)', context).addClass('advpoll-drag-and-drop-processed').each(function() {
    var mainForm = $(this);
    // Loop through the choices.
    $(".vote-choices div.form-item", $(this))
      .wrapAll("<ul class='vote-list' style='cursor: pointer'></ul>")
      .parent().parent().prepend('<div class="choice-header">' + Drupal.t("Choices") + '</div>')
      .end().end()
      .wrap("<li></li>").removeClass("form-item").parent().parent()
      .sortable({connectWith: [".advpoll-vote-list", "ol"]})
      .end().end().each(function() {
        $(this).parent().attr("id", "choice-" + parseInt($(this).attr("id").replace(/[^0-9]/g, "")));
        // Take off the annoying colon that Drupal adds to each title.
        $(this).html($(this).html().replace(/: <\/label>/ig, ''));
        $(this).parent().parent().siblings().filter(".writein-choice").clone().insertAfter($("select.advpoll-writeins", mainForm));
        // Hide the selects.
        $("select", $(this)).remove(); //("display", "none");
    });

    var maxChoices = parseInt($(this).find('input[name=max_choices]').val());
    var voteList = $('<ol class="advpoll-vote-list"></ol>').sortable({
        connectWith: [".vote-list", "ul"],
        placeholder: "choice-placeholder",
        drop: function(ev, ui) {
      },
      receive: function(ev, ui) {
        pseudoDeactivate(this, ev, ui, "receive");
        var mainItem = this;
        $(ui.item).not(".has-advpoll-x").addClass("has-advpoll-x").find("label").append($(' <span class="advpoll-x">(<a href="#" title="' + Drupal.t("Remove choice") + '">x</a>)</span>').click(function() {
          $(ui.item).removeClass("has-advpoll-x");
          $(".vote-list", mainForm).append($(this).parent().parent().find("span").remove().end().clone().end().parent().remove());
          // Call so that we update the number of remaining choices.
          pseudoDeactivate(mainItem, ev, ui, "remove");
          // Return false so that browser doesn't evaluate href and visit "#".
          return false;
        }));
      },
      remove: function(ev, ui) {
       $(ui.item).removeClass("has-advpoll-x").find(".advpoll-x").remove();
        pseudoDeactivate(this, ev, ui, "remove");
      }});

     function pseudoDeactivate(obj, ev, ui, eventType) {
        // Have to subtract 1 when receiving a new choice due to the placeholder element also being included in the count.
        var currentChoices = $("li", $(obj)).size() - (eventType == "receive" ? 1 : 0);
        $(".vote-status", mainForm).show().html(Drupal.t("Choices remaining: %choices", {"%choices" : maxChoices - currentChoices}));
        if (currentChoices > maxChoices) {
          // Don't allow more votes if we have hit the limit.
          // Doesn't seem to work: $(this).sortable({connectWith: ""});
          $(".vote-status", mainForm).addClass("error");
          newVoteButton.attr("disabled", "true");
        }
        else {
          newVoteButton.attr("disabled", "");
          $(".vote-status", mainForm).removeClass("error");
        }

        // If we went from 0 choices to 1, enable the vote button.
        if (currentChoices == 1) {
          newVoteButton.attr("disabled", "");
        }
        else if (currentChoices == 0) {
          // Back at 0, so the user can't cast a vote.
          newVoteButton.attr("disabled", "true");
        }
        var results = $("<span></span>");
        var order = 1;
        var votes = $(obj).sortable("toArray");
        // Output the sorted vote as hidden inputs so we can use the normal form handling code.
        for (var choice in votes) {
          if (votes[choice]) {
            results.append('<input type="hidden" name="'+ votes[choice].replace(/-(\d+)/, "[$1]") + '" value="'+ order + '" />');
            order++;
          }
        }
        $(".vote-results", mainForm).html(results);
      }

    var voteButton = $(".form-submit", $(this));
    var newVoteButton = voteButton.clone().attr("disabled", "true").css("margin-left", "10px").css("margin-top", "4px").addClass("vote-button");
    // Create a droppable area.
    $(this).append('<div class="advpoll-drag-box"><div class="advpoll-vote-header">'+ Drupal.t('Your Vote') + '</div><div class="advpoll-vote-choices"></div></div>')
      .find(".advpoll-vote-choices").append(voteList)
      .append(newVoteButton);
    // Remove the old vote button
    voteButton.remove();
    $('<div class="vote-status"></div>').insertAfter(newVoteButton).hide();
    $(this).append("<br clear='left' />").append('<div class="vote-results"></div>');

    // Show the write-in box if it exists.
    var newInput = $(".writein-choice input", mainForm).clone().css("display", "inline");
    $(".writein-choice").remove();
    $("li:last", mainForm).append(newInput);
  });
};

