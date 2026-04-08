import { useState, useEffect } from "react";
import { submitLicense, onLicenseStatus } from "../bridge";
import { Button } from "@/components/ui/button";
import { Textarea } from "@/components/ui/textarea";
import { Label } from "@/components/ui/label";
import {
  Card,
  CardContent,
  CardDescription,
  CardFooter,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";

export default function LicenseOverlay({ onActivated }) {
  const [keyInput, setKeyInput] = useState("");
  const [error, setError] = useState(false);

  useEffect(() => {
    return onLicenseStatus((status) => {
      if (status.valid) {
        onActivated?.();
      } else if (keyInput.length > 0) {
        setError(true);
      }
    });
  }, [keyInput, onActivated]);

  const handleActivate = () => {
    setError(false);
    submitLicense(keyInput.trim());
  };

  return (
    <div className="absolute inset-0 z-[100] flex items-center justify-center rounded-[inherit] bg-black/88">
      <Card className="w-[320px] bg-card border-border">
        <CardHeader className="pb-3">
          <CardTitle className="text-sm tracking-wide">Activate Degradizer</CardTitle>
          <CardDescription className="text-xs">
            Paste the license key you received after purchase.
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-3">
          <div className="space-y-1.5">
            <Label htmlFor="license-key" className="sr-only">License key</Label>
            <Textarea
              id="license-key"
              value={keyInput}
              onChange={(e) => { setKeyInput(e.target.value); setError(false); }}
              onKeyDown={(e) => { if (e.key === "Enter" && !e.shiftKey) { e.preventDefault(); handleActivate(); } }}
              placeholder="Paste license key here…"
              className={`h-16 resize-none font-mono text-[10px] bg-input ${error ? "border-destructive focus-visible:ring-destructive" : ""}`}
            />
            {error && (
              <p className="text-[11px] text-destructive">
                Invalid license key. Please check and try again.
              </p>
            )}
          </div>
        </CardContent>
        <CardFooter>
          <Button onClick={handleActivate} variant="secondary" className="w-full text-sm">
            Activate
          </Button>
        </CardFooter>
      </Card>
    </div>
  );
}
